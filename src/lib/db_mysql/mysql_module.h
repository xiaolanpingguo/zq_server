#pragma once

#include "lib/interface_header/IClassModule.h"
#include "lib/interface_header/ILogModule.h"
#include "lib/interface_header/IElementModule.h"
#include "lib/interface_header/IMysqlModule.h"

#include "database_worker_pool.h"
#include "query_callback.h"

namespace zq {

//class TestTable : public MySQLConnection
//{
//public:
//
//	enum TestTableStatements : uint32
//	{
//		TEST_1,
//		TEST_2,
//		TEST_MAX
//	};
//
//	using Statements = TestTableStatements;
//	TestTable(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
//	TestTable(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}
//	~TestTable() {}
//
//	void doPrepareStatements() override
//	{
//		if (!isReconnecting_)
//			vecStmts_.resize(TestTableStatements::TEST_MAX);
//
//		prepareStatement(TestTableStatements::TEST_1, "insert into person(age, name) value(?, ?)", CONNECTION_ASYNC);
//		prepareStatement(TestTableStatements::TEST_2, "update person set age = ? where name = ?", CONNECTION_SYNCH);
//	}
//};
//
//extern DatabaseWorkerPool<TestTable> g_TestTable;


// 想了很久，这个模块还是没找到很好的一个设计
// 因为按照现在这个模块化设计的话，不应该出现全局变量，但是
// 这里我暂时没想到一个又易用又好的接口的设计办法，暂时采用一个表是一个
// 全局变量的形式，这种方式偏向于易用性
class DatabaseLoader;
class QueryCallbackProcessor;
class MysqlModule : public IMysqlModule
{
	using DatabaseLoaderPtr = std::unique_ptr<DatabaseLoader>;
	using QueryCallbackProcessorPtr = std::unique_ptr<QueryCallbackProcessor>;
public:

	MysqlModule(ILibManager* p);
	virtual ~MysqlModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool finalize() override;

public:


protected:

	DatabaseLoaderPtr dbLoader_;
	QueryCallbackProcessorPtr queryProcessor_;

	IElementModule * elementModule_;
	IClassModule* classModule_;
	ILogModule* logModule_;
};


}
