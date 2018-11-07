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

	// ͬ��
	MySQLConnection(MySQLConnectionInfo& connInfo);  

	// �첽
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

	// ��ִ��һ��������ʱ�򣬸�һ�����ӳ�������
	bool lockIfReady();

	// �ڲ���ִ����ɺ���worker_pool���ã�������ִ����������
	void unlock();

	MYSQL* getHandle() { return mysqlHandle_; }
	MySQLPreparedStatement* getPreparedStatement(uint32 index);
	void prepareStatement(uint32 index, const char* sql, ConnectionFlags flags);

	virtual void doPrepareStatements() {};

protected:

	// �Ƿ�������
	bool isReconnecting_; 

	// ��׼��stetment��ʱ���Ƿ��д���
	bool prepareError_;  

	// �Ѿ��ɹ�ִ�е�����Ԥ��ѯ
	std::vector<std::unique_ptr<MySQLPreparedStatement>> vecStmts_; 

	// ���е�Ԥ��ѯ
	PreparedStatementMap mapQueries_;

private:
	bool _handleMySQLErrno(uint32 errNo, uint8 attempts = 5);

private:

	// ����Ϣ
	MySQLConnectionInfo& connectionInfo_;             

	// ͬ�������첽�ı�־������Ԥ��ѯ��ʱ��
	ConnectionFlags connectionFlags_;   

	// �̰߳�ȫ�������������߶��У����ָ���Ǵ��ⲿ���ݹ�������worker_pool���е�new
	ProducerConsumerQueue<SQLOperation*>* queue_;

	// �������̣߳�������߳���ȡ������
	std::unique_ptr<DatabaseWorker> worker_;  

	MYSQL* mysqlHandle_;

	std::mutex mutex_;

	MySQLConnection(MySQLConnection const& right) = delete;
	MySQLConnection& operator=(MySQLConnection const& right) = delete;
};

}



