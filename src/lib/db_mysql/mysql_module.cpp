#include "mysql_module.h" 

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <WinSock2.h>
#pragma comment(lib, "libmysql.lib")	
#endif
#include "database_loader.h"
#include "query_callback_processor.h"
#include <mysql.h>

namespace zq {


MysqlModule::MysqlModule(ILibManager* p)
{
	libManager_ = p;
}

MysqlModule::~MysqlModule()
{

}

bool MysqlModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	elementModule_ = libManager_->findModule<IElementModule>();
	logModule_ = libManager_->findModule<ILogModule>();

	mysql_library_init(-1, NULL, NULL);

	dbLoader_ = std::make_unique<DatabaseLoader>();
	queryProcessor_ = std::make_unique<QueryCallbackProcessor>();
	//dbLoader_->addDatabase(g_TestTable, "127.0.0.1;3306;root;8292340;test_table1");

	return true;
}

bool MysqlModule::initEnd()
{

	return true;
}

bool MysqlModule::finalize()
{
	mysql_library_end();
	return true;
}

bool MysqlModule::run()
{
	queryProcessor_->processReadyQueries();
	return true;
}


}