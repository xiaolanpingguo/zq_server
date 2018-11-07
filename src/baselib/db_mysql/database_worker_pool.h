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

// ping����
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

		// ͬ��
		uint32 error = openConnections(IDX_ASYNC, async_threads_);
		if (error)
		{
			return error;
		}

		// �첽
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

		// �ر�mysql��ͬ������.
		connections_[IDX_ASYNC].clear();

		// �ر��첽�����ӣ��˺���������Ҫ�������߳��˳�ʱ��ŵ���
		connections_[IDX_SYNCH].clear();

		LOG_INFO << "All connections on DatabasePool" << getDatabaseName() << " closed.";
	}

	// ׼�����е�Ԥִ�����
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

	// ͬ��
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

	// ͬ���� �������CONNECTION_SYNCH��־
	bool syncExecute(PreparedStatement* stmt)
	{
		if (stmt == nullptr)
		{
			return false;
		}

		T* connection = getFreeConnection();
		connection->execute(stmt);
		connection->unlock();

		// �ǵ�ɾ�ڴ�
		delete stmt;

		return true;
	}

	// ͬ�������ؽ��
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

	// ͬ�������ؽ��, �������CONNECTION_SYNCH��־
	PreparedQueryResult syncExecuteWithResult(PreparedStatement* stmt)
	{
		auto connection = getFreeConnection();
		PreparedResultSet* ret = connection->query(stmt);

		// ��ѯ��ϣ�����
		connection->unlock();

		// �ǵ�ɾ�ڴ�
		delete stmt;

		if (!ret || !ret->getRowCount())
		{
			delete ret;
			return nullptr;
		}

		return PreparedQueryResult(ret);
	}

	// �첽
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

	// �첽���������CONNECTION_ASYNC��־
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

	// �첽�����ؽ��
	QueryCallback asyncExecuteWithResult(const char* sql)
	{
		BasicStatementTask* task = new BasicStatementTask(sql, true);
		
		// ������Ҫ�Ȼ��getFuture����Ȼ���ܵ��º����˳�֮ǰ���Ѿ�ִ����������ɾ����
		QueryResultFuture result = task->getFuture();
		enqueue(task);
		return QueryCallback(std::move(result));
	}

	// �첽�����ؽ�����������CONNECTION_ASYNC��־
	QueryCallback asyncExecuteWithResult(PreparedStatement* stmt)
	{
		PreparedStatementTask* task = new PreparedStatementTask(stmt, true);
		
		// ������Ҫ�Ȼ��getFuture����Ȼ���ܵ��º����˳�֮ǰ���Ѿ�ִ����������ɾ����
		PreparedQueryResultFuture result = task->getFuture();
		enqueue(task);
		return QueryCallback(std::move(result));
	}

	// �첽ִ�ж������
	QueryResultHolderFuture delayQueryHolder(SQLQueryHolder* holder)
	{
		SQLQueryHolderTask* task = new SQLQueryHolderTask(holder);
		
		// ������Ҫ�Ȼ��getFuture����Ȼ���ܵ��º����˳�֮ǰ���Ѿ�ִ����������ɾ����
		QueryResultHolderFuture result = task->getFuture();
		enqueue(task);
		return result;
	}

	// ��ʼһ�������������Autocommit=0�������Զ��ع�
	SQLTransaction beginTransaction()
	{
		return std::make_shared<Transaction>();
	}

	// �ύһ��sql�Ĳ���(sql������prepare,holder)����Щ��������˳��ִ��
	void commitTransaction(SQLTransaction transaction)
	{
		// ������
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

	// ֱ���ύһ��sql�Ĳ���(sql������prepare,holder)����Щ��������˳��ִ��
	void directCommitTransaction(SQLTransaction& transaction)
	{
		T* connection = getFreeConnection();
		int errorCode = connection->executeTransaction(transaction);
		if (!errorCode)
		{
			// �����ɹ�������
			connection->unlock();     
			return;
		}

		// ������
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

		// ���
		transaction->cleanup();

		connection->unlock();
	}

	// �������������װ�ķ�ʽ����
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

	// ���һ������Ҫִ�е���䣬ָ�뽫�� 
	// this->query(PreparedStatement*) ��PreparedStatementTask::~PreparedStatementTasksɾ��
	PreparedStatement* getPreparedStatement(uint32 index)
	{
		return new PreparedStatement(index);
	}

	// ���ַ�������ת�����
	void escapeString(std::string& str)
	{
		if (str.empty())
			return;

		char* buf = new char[str.size() * 2 + 1];
		escapeString(buf, str.c_str(), uint32(str.size()));
		str = buf;
		delete[] buf;
	}

	// ����
	void keepAlive()
	{
		// ͬ���ķ�ʽping
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
		// �߳���,�ж����߳̾ʹ������ٸ�����
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

			// �������ǿ�ʼ����mysql
			if (uint32 error = connection->open())
			{
				// ����ʧ�ܣ���������ֱ��������Ӳ��ҷ���
				connections_[type].clear();
				return error;
			}
			// �����汾���
			else if (mysql_get_server_version(connection->getHandle()) < MIN_MYSQL_SERVER_VERSION)
			{
				LOG_ERROR << "server does not support MySQL versions below 5.1";
				return 1;
			}
			// ���ӳɹ�
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

	//! ���һ�����õ����ӣ��ǵ�Ҫ�ڲ���ִ����ɺ󣬵���t->unlock()����
	T* getFreeConnection()
	{
		uint8 i = 0;
		auto const num_cons = connections_[IDX_SYNCH].size();
		T* connection = nullptr;
		
		// ������Զִ�У�ֱ���п��е�����
		for (;;)
		{
			connection = connections_[IDX_SYNCH][i++ % num_cons].get();

			// �ǵ����������Ҫunlock()��������Ȼ������
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

	// �̰߳�ȫ�������������߶��У������첽ģʽ����������һ�������ߣ����������
	std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> queue_;

	// �����Ϊ������ģʽ��һ��ͬ����һ���첽��Ȼ��ÿ��ģʽ��������ж������
	// ͬ��ģʽ�ÿ�β�ѯ�����һ�����е����ӣ�Ȼ���ѯ�����У���mutex����������
	// ֱ����ѯ��Ϻ󣬽���
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;

	// table�������Ϣ
	std::unique_ptr<MySQLConnectionInfo> connectionInfo_;

	// �߳���
	uint8 async_threads_, synch_threads_;
};

}



