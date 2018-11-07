#include "mysql_connection.h"
#include "database_worker.h"
#include "prepared_statement.h"
#include "query_result.h"
#include "transaction.h"
#include "lib/base_code/util.h"
#include "sql_operation.h"

#ifdef _WIN32 // hack for broken mysql.h not including the correct winsock header for SOCKET definition, fixed in 5.7
#include <winsock2.h>
#endif
#include <errmsg.h>
#include <mysql.h>
#include <mysqld_error.h>

namespace zq {

MySQLConnectionInfo::MySQLConnectionInfo(std::string const& infoString)
{
	util::Tokenizer tokens(infoString, ';');

	if (tokens.size() != 5)
		return;

	uint8 i = 0;

	host.assign(tokens[i++]);
	port_or_socket.assign(tokens[i++]);
	user.assign(tokens[i++]);
	password.assign(tokens[i++]);
	database.assign(tokens[i++]);
}

MySQLConnection::MySQLConnection(MySQLConnectionInfo& connInfo) :
	isReconnecting_(false),
	prepareError_(false),
	queue_(nullptr),
	mysqlHandle_(nullptr),
	connectionInfo_(connInfo),
	connectionFlags_(CONNECTION_SYNCH) 
{
}

MySQLConnection::MySQLConnection(ProducerConsumerQueue<SQLOperation*>* queue, MySQLConnectionInfo& connInfo) :
	isReconnecting_(false),
	prepareError_(false),
	queue_(queue),
	mysqlHandle_(nullptr),
	connectionInfo_(connInfo),
	connectionFlags_(CONNECTION_ASYNC)
{
	worker_ = std::make_unique<DatabaseWorker>(queue_, this);
}

MySQLConnection::~MySQLConnection()
{
	close();
}

void MySQLConnection::close()
{
	// 先停止工作者线程，再清除vecStmts_
	worker_.reset();

	vecStmts_.clear();

	if (mysqlHandle_)
	{
		mysql_close(mysqlHandle_);
		mysqlHandle_ = nullptr;
	}
}

uint32 MySQLConnection::open()
{
	MYSQL *mysqlInit;
	mysqlInit = mysql_init(NULL);
	if (!mysqlInit)
	{
		return CR_UNKNOWN_ERROR;
	}

	int port;
	char const* unix_socket;
	//unsigned int timeout = 10;

	mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8mb4");
	//mysql_options(mysqlInit, MYSQL_OPT_READ_TIMEOUT, (char const*)&timeout);
#ifdef _WIN32
	if (connectionInfo_.host == ".")                                          
	{
		// 如果没填地址的话，就采用管道的方式
		unsigned int opt = MYSQL_PROTOCOL_PIPE;
		mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, (char const*)&opt);
		port = 0;
		unix_socket = 0;
	}
	else                                                   
	{
		port = atoi(connectionInfo_.port_or_socket.c_str());
		unix_socket = 0;
	}
#else
	if (connectionInfo_.host == ".")                                    
	{
		unsigned int opt = MYSQL_PROTOCOL_SOCKET;
		mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, (char const*)&opt);
		connectionInfo_.host = "localhost";
		port = 0;
		unix_socket = connectionInfo_.port_or_socket.c_str();
	}
	else                                                
	{
		port = atoi(connectionInfo_.port_or_socket.c_str());
		unix_socket = nullptr;
	}
#endif

	mysqlHandle_ = mysql_real_connect(mysqlInit, connectionInfo_.host.c_str(), connectionInfo_.user.c_str(),
		connectionInfo_.password.c_str(), connectionInfo_.database.c_str(), port, unix_socket, 0);
	if (mysqlHandle_)
	{
		if (!isReconnecting_)
		{
			LOG_INFO << "mysql client library: " << mysql_get_client_info();
			LOG_INFO << "mysql server version: %s " << mysql_get_server_info(mysqlHandle_);

			// 如果两方版本不一样，这里打一个警告
			if (mysql_get_server_version(mysqlHandle_) != mysql_get_client_version())
			{
				LOG_WARN << "mysql client/server version mismatch; may conflict with behaviour of prepared statements.";
			}
		}

		// 自动提交事务
		mysql_autocommit(mysqlHandle_, 1);

		// 字符集
		mysql_set_character_set(mysqlHandle_, "utf8mb4");

		LOG_INFO << "Connected to MySQL database at " << connectionInfo_.host;
		return 0;
	}
	else
	{
		LOG_ERROR << "Could not connect to mysql database at: " << connectionInfo_.host << ",error: "<< mysql_error(mysqlInit);
		mysql_close(mysqlInit);
		return mysql_errno(mysqlInit);
	}
}

bool MySQLConnection::prepareStatements()
{
	doPrepareStatements();
	return !prepareError_;
}

bool MySQLConnection::execute(const char* sql)
{
	if (!mysqlHandle_ || sql == nullptr)
	{
		return false;
	}

	{
		uint32 _s = getMSTime();

		if (mysql_query(mysqlHandle_, sql))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);

			LOG_ERROR << "sql error, sql: " << sql << ", error_str: " << mysql_error(mysqlHandle_);

			if (_handleMySQLErrno(lErrno))
				return execute(sql);     

			return false;
		}
		else
		{
			LOG_INFO << "this query, sql: " << sql
				<< ", cost time: " << getMSTimeDiff(_s, getMSTime()) << " ms";
		}
	}

	return true;
}

bool MySQLConnection::execute(PreparedStatement* stmt)
{
	if (!mysqlHandle_ || !stmt)
	{
		return false;
	}

	uint32 index = stmt->m_index;
	{
		MySQLPreparedStatement* m_mStmt = getPreparedStatement(index);
		if (m_mStmt == nullptr)
		{
			return false;
		}

		m_mStmt->m_stmt = stmt;     // Cross reference them for debug output
		stmt->m_stmt = m_mStmt;     /// @todo Cleaner way

		stmt->bindParameters();

		MYSQL_STMT* msql_STMT = m_mStmt->getSTMT();
		MYSQL_BIND* msql_BIND = m_mStmt->getBind();

		uint32 _s = getMSTime();

		if (mysql_stmt_bind_param(msql_STMT, msql_BIND))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);

			LOG_ERROR << "sql error, sql:" << m_mStmt->getQueryString(mapQueries_[index].first).c_str()
				<< ",errorcode: " << lErrno << " error str " << mysql_stmt_error(msql_STMT);

			if (_handleMySQLErrno(lErrno)) 
				return execute(stmt);       

			m_mStmt->clearParameters();
			return false;
		}

		if (mysql_stmt_execute(msql_STMT))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);

			LOG_ERROR << "mysql_stmt_execute error, sql: " << m_mStmt->getQueryString(mapQueries_[index].first).c_str() <<
				",error_code: " << lErrno << " error_str: " << mysql_stmt_error(msql_STMT);

			if (_handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
				return execute(stmt);       // Try again

			m_mStmt->clearParameters();
			return false;
		}

		LOG_INFO << "this query, sql: " << m_mStmt->getQueryString(mapQueries_[index].first).c_str()
			<< ", cost time: " << getMSTimeDiff(_s, getMSTime()) << " ms";

		m_mStmt->clearParameters();
		return true;
	}
}

bool MySQLConnection::_query(PreparedStatement* stmt, MYSQL_RES** pResult, uint64* pRowCount, uint32* pFieldCount)
{
	if (!mysqlHandle_ || stmt == nullptr)
	{
		return false;
	}

	uint32 index = stmt->m_index;
	{
		// 如果为空的话，应该是用户调用错误，比如一个异步的执行语句用了
		// 同步的函数调用，反之亦成立
		MySQLPreparedStatement* m_mStmt = getPreparedStatement(index);
		if (m_mStmt == nullptr)
		{
			return false;
		}

		m_mStmt->m_stmt = stmt;     // Cross reference them for debug output
		stmt->m_stmt = m_mStmt;     /// @todo Cleaner way

		stmt->bindParameters();

		MYSQL_STMT* msql_STMT = m_mStmt->getSTMT();
		MYSQL_BIND* msql_BIND = m_mStmt->getBind();

		uint32 _s = getMSTime();

		if (mysql_stmt_bind_param(msql_STMT, msql_BIND))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);

			LOG_ERROR << "sql error, sql:" << m_mStmt->getQueryString(mapQueries_[index].first).c_str()
				<< "errorcode: " << lErrno << " error str " << mysql_stmt_error(msql_STMT);

			// 这里我们处理下这个错误，如果返回true,那就代表错误已经成功处理了，比如重连
			// 所以我们再来做一次查询
			if (_handleMySQLErrno(lErrno))
			{
				return _query(stmt, pResult, pRowCount, pFieldCount);
			}     

			m_mStmt->clearParameters();
			return false;
		}

		if (mysql_stmt_execute(msql_STMT))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);
			LOG_ERROR << "sql mysql_stmt_execute error, sql: " << m_mStmt->getQueryString(mapQueries_[index].first).c_str() <<
				"error_code: " << lErrno << " error_str: " << mysql_stmt_error(msql_STMT);

			// 这里我们处理下这个错误，如果返回true,那就代表错误已经成功处理了，比如重连
			// 所以我们再来做一次查询
			if (_handleMySQLErrno(lErrno))  
				return _query(stmt, pResult, pRowCount, pFieldCount);      

			m_mStmt->clearParameters();
			return false;
		}

		LOG_INFO << "this query, sql: " << m_mStmt->getQueryString(mapQueries_[index].first).c_str()
			<< "cost time: " << getMSTimeDiff(_s, getMSTime()) << " ms";

		m_mStmt->clearParameters();

		// 返回结果集元数据，该指针指向MYSQL_RES结构，可用于处理元信息，
		// 如总的字段数以及单独的字段信息,结果集仅包含元数据,不含任何行结果
		*pResult = mysql_stmt_result_metadata(msql_STMT);
		*pRowCount = mysql_stmt_num_rows(msql_STMT);
		*pFieldCount = mysql_stmt_field_count(msql_STMT);

		return true;

	}
}

ResultSet* MySQLConnection::query(const char* sql)
{
	if (!sql)
	{
		return nullptr;
	}

	MYSQL_RES *result = nullptr;
	MYSQL_FIELD *fields = nullptr;
	uint64 rowCount = 0;
	uint32 fieldCount = 0;

	if (!_query(sql, &result, &fields, &rowCount, &fieldCount))
	{
		return nullptr;
	}

	return new ResultSet(result, fields, rowCount, fieldCount);
}

bool MySQLConnection::_query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount)
{
	if (!mysqlHandle_ || sql == nullptr)
	{
		return false;
	}

	{
		uint32 _s = getMSTime();

		if (mysql_query(mysqlHandle_, sql))
		{
			uint32 lErrno = mysql_errno(mysqlHandle_);

			LOG_ERROR << "sql query error, sql: " << sql << "errrno: " << lErrno << ",error_str: " << mysql_error(mysqlHandle_);

			if (_handleMySQLErrno(lErrno))
			{
				return _query(sql, pResult, pFields, pRowCount, pFieldCount);
			}

			return false;
		}
		else
		{
			LOG_INFO << "this query, sql: " << sql << ",cost time: " << getMSTimeDiff(_s, getMSTime()) << " ms";
		}

		*pResult = mysql_store_result(mysqlHandle_);
		*pRowCount = mysql_affected_rows(mysqlHandle_);
		*pFieldCount = mysql_field_count(mysqlHandle_);
	}

	if (!*pResult)
		return false;

	if (!*pRowCount)
	{
		mysql_free_result(*pResult);
		return false;
	}

	*pFields = mysql_fetch_fields(*pResult);

	return true;
}

void MySQLConnection::beginTransaction()
{
	execute("START TRANSACTION");
}

void MySQLConnection::rollbackTransaction()
{
	execute("ROLLBACK");
}

void MySQLConnection::commitTransaction()
{
	execute("COMMIT");
}

int MySQLConnection::executeTransaction(SQLTransaction& transaction)
{
	std::vector<SQLElementData> const& queries = transaction->vecQueries_;
	if (queries.empty())
	{
		return -1;
	}

	beginTransaction();

	for (auto itr = queries.begin(); itr != queries.end(); ++itr)
	{
		SQLElementData const& data = *itr;
		switch (itr->type)
		{
		case SQL_ELEMENT_PREPARED:
		{
			PreparedStatement* stmt = data.element.stmt;
			ASSERT(stmt);
			if (!execute(stmt))
			{
				LOG_WARN << "Transaction aborted. " << queries.size() << "queries not executed.";

				int errorCode = getLastError();
				rollbackTransaction();
				return errorCode;
			}
		}
		break;
		case SQL_ELEMENT_RAW:
		{
			const char* sql = data.element.query;
			ASSERT(sql);
			if (!execute(sql))
			{
				LOG_WARN << "Transaction aborted. " << queries.size() << "queries not executed.";

				int errorCode = getLastError();
				rollbackTransaction();
				return errorCode;
			}
		}
		break;
		}
	}

	// we might encounter errors during certain queries, and depending on the kind of error
	// we might want to restart the transaction. So to prevent data loss, we only clean up when it's all done.
	// This is done in calling functions DatabaseWorkerPool<T>::directCommitTransaction and TransactionTask::execute,
	// and not while iterating over every element.

	commitTransaction();
	return 0;
}

void MySQLConnection::ping()
{
	mysql_ping(mysqlHandle_);
}

uint32 MySQLConnection::getLastError()
{
	return mysql_errno(mysqlHandle_);
}

bool MySQLConnection::lockIfReady()
{
	return mutex_.try_lock();
}

void MySQLConnection::unlock()
{
	mutex_.unlock();
}

MySQLPreparedStatement* MySQLConnection::getPreparedStatement(uint32 index)
{
	if (index >= vecStmts_.size())
	{
		return nullptr;
	}

	MySQLPreparedStatement* ret = vecStmts_[index].get();
	if (!ret)
	{
		LOG_ERROR << "Could not fetch prepared statement, index: " << index
			<< "on database " << connectionInfo_.database << ",type: " <<
			((connectionFlags_ & CONNECTION_ASYNC) ? "asynchronous" : "synchronous");
	}

	return ret;
}

void MySQLConnection::prepareStatement(uint32 index, const char* sql, ConnectionFlags flags)
{
	if (sql == nullptr || index >= vecStmts_.size())
	{
		return;
	}

	mapQueries_.insert(std::make_pair(index, std::make_pair(sql, flags)));

	// 检查标志，不能出现同步的连接出现异步的标志
	// 或者异步的连接出现同步的标志
	if (!(connectionFlags_ & flags))
	{
		vecStmts_[index].reset();
		return;
	}

	MYSQL_STMT* stmt = mysql_stmt_init(mysqlHandle_);
	if (!stmt)
	{
		LOG_ERROR << "In mysql_stmt_init() error id: " << index << ",sql: " << sql << ",errorstr: " << mysql_error(mysqlHandle_);

		prepareError_ = true;
	}
	else
	{
		if (mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(strlen(sql))))
		{
			LOG_ERROR << "In mysql_stmt_init() error id: " << index << ",sql: " << sql << ",errorstr: " << mysql_stmt_error(stmt);

			mysql_stmt_close(stmt);
			prepareError_ = true;
		}
		else
		{
			vecStmts_[index] = std::make_unique<MySQLPreparedStatement>(stmt);
		}
	}
}

PreparedResultSet* MySQLConnection::query(PreparedStatement* stmt)
{
	MYSQL_RES *result = NULL;
	uint64 rowCount = 0;
	uint32 fieldCount = 0;

	if (!_query(stmt, &result, &rowCount, &fieldCount))
		return NULL;

	// 如果存在多个结果，返回真，如果不存在多个结果，返回0
	// 如果返回真，应用程序必须调用mysql_next_result()来获取结果
	// 用于执行多条语句的时候
	if (mysql_more_results(mysqlHandle_))
	{
		mysql_next_result(mysqlHandle_);
	}

	return new PreparedResultSet(stmt->m_stmt->getSTMT(), result, rowCount, fieldCount);
}

bool MySQLConnection::_handleMySQLErrno(uint32 errNo, uint8 attempts /*= 5*/)
{
	switch (errNo)
	{
	case CR_SERVER_GONE_ERROR:
	case CR_SERVER_LOST:
	case CR_INVALID_CONN_HANDLE:
	case CR_SERVER_LOST_EXTENDED:
	{
		// 丢失连接，这里不用break
		if (mysqlHandle_)
		{
			LOG_ERROR << "Lost the connection to the MySQL server!";

			mysql_close(getHandle());
			mysqlHandle_ = nullptr;
		}
	}
	case CR_CONN_HOST_ERROR:
	{
		LOG_INFO << "Attempting to reconnect to the MySQL server...";

		isReconnecting_ = true;

		uint32 const lErrno = open();
		if (!lErrno)
		{
			// Don't remove 'this' pointer unless you want to skip loading all prepared statements...
			if (!this->prepareStatements())
			{
				LOG_ERROR << "Could not re-prepare statements!";
				std::this_thread::sleep_for(std::chrono::seconds(10));
				std::abort();
			}

			LOG_INFO << "successfully reconnected to %s @%s:%s (%s)." << connectionInfo_.database << " "
				<< connectionInfo_.host << " " << connectionInfo_.port_or_socket << " "
				<< ((connectionFlags_ & CONNECTION_ASYNC) ? "asynchronous" : "synchronous");

			isReconnecting_ = false;
			return true;
		}

		// 这里我们尝试几次，如果还连接不上，就认为mysql已经挂了
		// 此时就让server退出吧，因为就算再运行也没意义
		if ((--attempts) == 0)
		{
			LOG_ERROR << "failed to reconnect to the mysql server, terminating the server...";

			// std::raise(SIGTERM)
			std::this_thread::sleep_for(std::chrono::seconds(10));
			std::abort();
		}
		else
		{
			// 这里可能会收到2006的错误，我们继续用递归的方式处理这个错误
			// 为了防止递归爆栈，我们睡眠几秒
			std::this_thread::sleep_for(std::chrono::seconds(3)); 
			return _handleMySQLErrno(lErrno, attempts); 
		}
	}

	// 死锁了，详情用TransactionTask::execute and DatabaseWorkerPool<T>::directCommitTransaction
	// 这个查询先丢掉
	case ER_LOCK_DEADLOCK:
		return false;    

	case ER_WRONG_VALUE_COUNT:
	case ER_DUP_ENTRY:
		return false;

	// 表错误，或者找不到这个表
	case ER_BAD_FIELD_ERROR:
	case ER_NO_SUCH_TABLE:
		LOG_ERROR << "Your database structure is Outdated.";
		std::this_thread::sleep_for(std::chrono::seconds(10));
		std::abort();
		return false;
	case ER_PARSE_ERROR:
		LOG_ERROR << "Error while parsing SQL. Core fix required.";
		std::this_thread::sleep_for(std::chrono::seconds(10));
		std::abort();
		return false;
	default:
		LOG_ERROR << "Unhandled MySQL errno: " << errNo;
		return false;
	}
}

}


