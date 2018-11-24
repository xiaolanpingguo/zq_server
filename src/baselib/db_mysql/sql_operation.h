#pragma once

#include "database_env_fwd.h"

namespace zq {


class MySQLConnection;
class SQLOperation
{
public:
	SQLOperation(const char* sql) 
		:sql_(sql),
		conn_(nullptr) 
	{ }
	virtual ~SQLOperation() { }

	virtual int call()
	{
		execute();
		return 0;
	}
	virtual bool execute() {}
	virtual void setConnection(MySQLConnection* con) { conn_ = con; }

	MySQLConnection* conn_;

private:
	SQLOperation(SQLOperation const& right) = delete;
	SQLOperation& operator=(SQLOperation const& right) = delete;

	const char* sql_;
};


}



