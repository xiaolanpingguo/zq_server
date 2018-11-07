#pragma once

#include "interface_header/platform.h"
#include "database_env_fwd.h"
#include "basic_statement.h"
#include "prepared_statement.h"
#include "query_callback.h"
#include "query_holder.h"
#include "query_result.h"
#include "sql_operation.h"
#include "transaction.h"
#include "mysql_connection.h"
#include "baselib/base_code/producer_consumer_queue.hpp"

#ifdef _WIN32 // hack for broken mysql.h not including the correct winsock header for SOCKET definition, fixed in 5.7
#include <winsock2.h>
#endif
#include <mysql.h>
#include <mysqld_error.h>

#include <array>
#include <string>
#include <vector>

namespace zq {

#define MIN_MYSQL_SERVER_VERSION 50100u
#define MIN_MYSQL_CLIENT_VERSION 50100u

template <typename T>
class ProducerConsumerQueue;

class SQLOperation;
struct MySQLConnectionInfo;

// ping操作
class PingOperation : public SQLOperation
{
public:

	bool execute() override
	{
		conn_->ping();
		return true;
	}
};

template <class T>
class DatabaseWorkerPool
{
private:
	enum InternalIndex
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

public:

	DatabaseWorkerPool()
		: queue_(new ProducerConsumerQueue<SQLOperation*>()),
		async_threads_(0), synch_threads_(0)
	{
		ASSERT(mysql_thread_safe(), "Used MySQL library isn't thread-safe.");
		ASSERT(mysql_get_client_version() >= MIN_MYSQL_CLIENT_VERSION, "server does not support MySQL versions below 5.1");
		ASSERT(mysql_get_client_version() == MYSQL_VERSION_ID, "MySQL library version (%s) does not match the server version (%s).",
			mysql_get_client_info(), MYSQL_SERVER_VERSION);
	}
	~DatabaseWorkerPool()
	{
		queue_->cancel();
	}

	void setConnectionInfo(std::string const& infoString, uint8 const asyncThreads, uint8 const synchThreads)
	{
		connectionInfo_ = std::make_unique<MySQLConnectionInfo>(infoString);

		async_threads_ = asyncThreads;
		synch_threads_ = synchThreads;
	}

	uint32 open()
	{
		if (!connectionInfo_)
		{
			LOG_ERROR << "connectionInfo_ was not ser";
			return -1;
		}

		LOG_INFO << "Opening DatabasePool, name: " << getDatabaseName()
			<< "async_threads_: " << async_threads_
			<< "synch_threads_" << synch_threads_;

		// 同步
		uint32 error = openConnections(IDX_ASYNC, async_threads_);
		if (error)
		{
			return error;
		}

		// 异步
		error = openConnections(IDX_SYNCH, synch_threads_);
		if (!error)
		{
			LOG_INFO <<"open sync DatabasePool " << getDatabaseName() << " opened successfully.";
		}

		return error;
	}

	void close()
	{
		LOG_INFO << "Closing down DatabasePool:" << getDatabaseName();

		// 关闭mysql的同步连接.
		connections_[IDX_ASYNC].clear();

		// 关闭异步的连接，此函数调必须要在其他线程退出时候才调用
		connections_[IDX_SYNCH].clear();

		LOG_INFO << "All connections on DatabasePool" << getDatabaseName() << " closed.";
	}

	// 准备所有的预执行语句
	bool prepareStatements()
	{
		for (auto& connections : connections_)
		{
			for (auto& connection : connections)
			{
				connection->lockIfReady();
				if (!connection->prepareStatements())
				{
					connection->unlock();
					close();
					return false;
				}
				else
				{
					connection->unlock();
				}
			}
		}

		return true;
	}

	inline MySQLConnectionInfo const* getConnectionInfo() const
	{
		return connectionInfo_.get();
	}

	// 同步
	bool syncExecute(const char* sql)
	{
		if (sql == nullptr)
		{
			return false;
		}

		T* connection = getFreeConnection();
		connection->execute(sql);
		connection->unlock();

		return true;
	}

	// 同步， 必须带有CONNECTION_SYNCH标志
	bool syncExecute(PreparedStatement* stmt)
	{
		if (stmt == nullptr)
		{
			return false;
		}

		T* connection = getFreeConnection();
		connection->execute(stmt);
		connection->unlock();

		// 记得删内存
		delete stmt;

		return true;
	}

	// 同步，返回结果
	QueryResult syncExecuteWithResult(const char* sql)
	{
		T* connection = getFreeConnection();
		ResultSet* result = connection->query(sql);
		connection->unlock();
		if (!result || !result->getRowCount() || !result->nextRow())
		{
			delete result;
			return QueryResult(NULL);
		}

		return QueryResult(result);
	}

	// 同步，返回结果, 必须带有CONNECTION_SYNCH标志
	PreparedQueryResult syncExecuteWithResult(PreparedStatement* stmt)
	{
		auto connection = getFreeConnection();
		PreparedResultSet* ret = connection->query(stmt);

		// 查询完毕，解锁
		connection->unlock();

		// 记得删内存
		delete stmt;

		if (!ret || !ret->getRowCount())
		{
			delete ret;
			return nullptr;
		}

		return PreparedQueryResult(ret);
	}

	// 异步
	bool asyncExecute(const char* sql)
	{
		if (sql == nullptr)
		{
			return false;
		}

		BasicStatementTask* task = new BasicStatementTask(sql);
		enqueue(task);

		return true;
	} 

	// 异步，必须带有CONNECTION_ASYNC标志
	bool asyncExecute(PreparedStatement* stmt)
	{
		if (stmt == nullptr)
		{
			return false;
		}

		PreparedStatementTask* task = new PreparedStatementTask(stmt);
		enqueue(task);

		return true;
	}

	// 异步，返回结果
	QueryCallback asyncExecuteWithResult(const char* sql)
	{
		BasicStatementTask* task = new BasicStatementTask(sql, true);
		
		// 这里需要先获得getFuture，不然可能导致函数退出之前就已经执行完任务并且删除了
		QueryResultFuture result = task->getFuture();
		enqueue(task);
		return QueryCallback(std::move(result));
	}

	// 异步，返回结果，必须带有CONNECTION_ASYNC标志
	QueryCallback asyncExecuteWithResult(PreparedStatement* stmt)
	{
		PreparedStatementTask* task = new PreparedStatementTask(stmt, true);
		
		// 这里需要先获得getFuture，不然可能导致函数退出之前就已经执行完任务并且删除了
		PreparedQueryResultFuture result = task->getFuture();
		enqueue(task);
		return QueryCallback(std::move(result));
	}

	// 异步执行多条语句
	QueryResultHolderFuture delayQueryHolder(SQLQueryHolder* holder)
	{
		SQLQueryHolderTask* task = new SQLQueryHolderTask(holder);
		
		// 这里需要先获得getFuture，不然可能导致函数退出之前就已经执行完任务并且删除了
		QueryResultHolderFuture result = task->getFuture();
		enqueue(task);
		return result;
	}

	// 开始一个事务，如果设置Autocommit=0，将会自动回滚
	SQLTransaction beginTransaction()
	{
		return std::make_shared<Transaction>();
	}

	// 提交一个sql的操作(sql语句或者prepare,holder)，这些操作都是顺序执行
	void commitTransaction(SQLTransaction transaction)
	{
		// 调试用
		switch (transaction->getSize())
		{
		case 0:
			LOG_INFO << "Transaction contains 0 queries. Not executing.";
			return;
		case 1:
			LOG_INFO << "Warning: Transaction only holds 1 query, consider removing Transaction context in code.";
			break;
		default:
			break;
		}

		enqueue(new TransactionTask(transaction));
	}

	// 直接提交一个sql的操作(sql语句或者prepare,holder)，这些操作都是顺序执行
	void directCommitTransaction(SQLTransaction& transaction)
	{
		T* connection = getFreeConnection();
		int errorCode = connection->executeTransaction(transaction);
		if (!errorCode)
		{
			// 操作成功，解锁
			connection->unlock();     
			return;
		}

		// 死锁了
		if (errorCode == ER_LOCK_DEADLOCK)
		{
			uint8 loopBreaker = 5;
			for (uint8 i = 0; i < loopBreaker; ++i)
			{
				if (!connection->executeTransaction(transaction))
				{
					break;
				}
			}
		}

		// 清除
		transaction->cleanup();

		connection->unlock();
	}

	// 将操作已事务封装的方式进行
	void executeOrAppend(SQLTransaction& trans, const char* sql)
	{
		if (!trans)
		{
			//execute(sql);
		}
		else
		{
			trans->append(sql);
		}
	}
	void executeOrAppend(SQLTransaction& trans, PreparedStatement* stmt)
	{
		if (!trans)
		{
			//execute(stmt);
		}
		else
		{
			trans->append(stmt);
		}
	}


	typedef typename T::Statements PreparedStatementIndex;

	// 获得一个即将要执行的语句，指针将在 
	// this->query(PreparedStatement*) 和PreparedStatementTask::~PreparedStatementTasks删除
	PreparedStatement* getPreparedStatement(uint32 index)
	{
		return new PreparedStatement(index);
	}

	// 将字符串进行转义编码
	void escapeString(std::string& str)
	{
		if (str.empty())
			return;

		char* buf = new char[str.size() * 2 + 1];
		escapeString(buf, str.c_str(), uint32(str.size()));
		str = buf;
		delete[] buf;
	}

	// 心跳
	void keepAlive()
	{
		// 同步的方式ping
		for (auto& connection : connections_[IDX_SYNCH])
		{
			if (connection->lockIfReady())
			{
				connection->ping();
				connection->unlock();
			}
		}

		auto const count = connections_[IDX_ASYNC].size();
		for (uint8 i = 0; i < count; ++i)
		{
			enqueue(new PingOperation);
		}
	}

private:
	uint32 openConnections(InternalIndex type, uint8 numConnections)
	{
		// 线程数,有多少线程就创建多少个连接
		for (uint8 i = 0; i < numConnections; ++i)
		{
			std::unique_ptr<T> connection;
			if (type == IDX_ASYNC)
			{
				connection = std::make_unique<T>(queue_.get(), *connectionInfo_);
			}
			else if (type == IDX_SYNCH)
			{
				connection = std::make_unique<T>(*connectionInfo_);
			}
			else
			{
				LOG_ERROR << "type error, type: " << type;
				return 1;
			}

			// 现在我们开始连接mysql
			if (uint32 error = connection->open())
			{
				// 连接失败，这里我们直接清楚连接并且返回
				connections_[type].clear();
				return error;
			}
			// 做个版本检查
			else if (mysql_get_server_version(connection->getHandle()) < MIN_MYSQL_SERVER_VERSION)
			{
				LOG_ERROR << "server does not support MySQL versions below 5.1";
				return 1;
			}
			// 连接成功
			else
			{
				connections_[type].push_back(std::move(connection));
			}
		}

		return 0;
	}

	unsigned long escapeString(char *to, const char *from, unsigned long length)
	{
		if (!to || !from || !length)
		{
			return 0;
		}

		return mysql_real_escape_string(connections_[IDX_SYNCH].front()->getHandle(), to, from, length);
	}

	void enqueue(SQLOperation* op)
	{
		queue_->push(op);
	}

	//! 获得一个可用的连接，记得要在操作执行完成后，调用t->unlock()解锁
	T* getFreeConnection()
	{
		uint8 i = 0;
		auto const num_cons = connections_[IDX_SYNCH].size();
		T* connection = nullptr;
		
		// 这里永远执行，直到有空闲的连接
		for (;;)
		{
			connection = connections_[IDX_SYNCH][i++ % num_cons].get();

			// 记得连接用完后要unlock()解锁，不然会死锁
			if (connection->lockIfReady())
			{
				break;
			}
		}

		return connection;
	}

	char const* getDatabaseName() const
	{
		return connectionInfo_->database.c_str();
	}

private:

	// 线程安全的生产者消费者队列，用于异步模式，在这里是一个生产者，多个消费者
	std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> queue_;

	// 这里分为了两种模式，一个同步，一个异步，然后每种模式里面可以有多个连接
	// 同步模式里，每次查询都获得一个空闲的连接，然后查询过程中，用mutex进行上锁，
	// 直到查询完毕后，解锁
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;

	// table的相关信息
	std::unique_ptr<MySQLConnectionInfo> connectionInfo_;

	// 线程数
	uint8 async_threads_, synch_threads_;
};

}



