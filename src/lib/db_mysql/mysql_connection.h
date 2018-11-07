#pragma once

#include "lib/interface_header/platform.h"
#include "database_env_fwd.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace zq {


template <typename T>
class ProducerConsumerQueue;

class DatabaseWorker;
class MySQLPreparedStatement;
class SQLOperation;

enum ConnectionFlags
{
	CONNECTION_ASYNC = 0x1,
	CONNECTION_SYNCH = 0x2,
	CONNECTION_BOTH = CONNECTION_ASYNC | CONNECTION_SYNCH
};

struct MySQLConnectionInfo
{
	explicit MySQLConnectionInfo(std::string const& infoString);

	std::string user;
	std::string password;
	std::string database;
	std::string host;
	std::string port_or_socket;
};

using PreparedStatementMap = std::map<uint32 /*index*/, std::pair<std::string /*query*/, ConnectionFlags /*sync/async*/>>;

template <typename T>
class DatabaseWorkerPool;


class MySQLConnection
{
	template <typename T> friend class DatabaseWorkerPool;
	friend class PingOperation;

public:

	// 同步
	MySQLConnection(MySQLConnectionInfo& connInfo);  

	// 异步
	MySQLConnection(ProducerConsumerQueue<SQLOperation*>* queue, MySQLConnectionInfo& connInfo); 
	virtual ~MySQLConnection();

	virtual uint32 open();
	void close();

	bool prepareStatements();

public:
	bool execute(const char* sql);
	bool execute(PreparedStatement* stmt);
	ResultSet* query(const char* sql);
	PreparedResultSet* query(PreparedStatement* stmt);
	bool _query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount);
	bool _query(PreparedStatement* stmt, MYSQL_RES **pResult, uint64* pRowCount, uint32* pFieldCount);

	void beginTransaction();
	void rollbackTransaction();
	void commitTransaction();
	int executeTransaction(SQLTransaction& transaction);

	void ping();

	uint32 getLastError();

protected:

	// 在执行一个操作的时候，给一个连接尝试上锁
	bool lockIfReady();

	// 在操作执行完成后，由worker_pool调用，继续来执行其他操作
	void unlock();

	MYSQL* getHandle() { return mysqlHandle_; }
	MySQLPreparedStatement* getPreparedStatement(uint32 index);
	void prepareStatement(uint32 index, const char* sql, ConnectionFlags flags);

	virtual void doPrepareStatements() {};

protected:

	// 是否在重连
	bool isReconnecting_; 

	// 在准备stetment的时候是否有错误
	bool prepareError_;  

	// 已经成功执行的所有预查询
	std::vector<std::unique_ptr<MySQLPreparedStatement>> vecStmts_; 

	// 所有的预查询
	PreparedStatementMap mapQueries_;

private:
	bool _handleMySQLErrno(uint32 errNo, uint8 attempts = 5);

private:

	// 表信息
	MySQLConnectionInfo& connectionInfo_;             

	// 同步还是异步的标志，用于预查询的时候
	ConnectionFlags connectionFlags_;   

	// 线程安全的生产者消费者队列，这个指针是从外部传递过来，在worker_pool进行的new
	ProducerConsumerQueue<SQLOperation*>* queue_;

	// 工作者线程，在这个线程中取出任务
	std::unique_ptr<DatabaseWorker> worker_;  

	MYSQL* mysqlHandle_;

	std::mutex mutex_;

	MySQLConnection(MySQLConnection const& right) = delete;
	MySQLConnection& operator=(MySQLConnection const& right) = delete;
};

}



