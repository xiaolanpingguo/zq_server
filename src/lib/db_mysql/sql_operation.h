#pragma once

#include "database_env_fwd.h"

namespace zq {

//- Union that holds element data
union SQLElementUnion
{
	PreparedStatement* stmt;
	const char* query;
};

//- Type specifier of our element data
enum SQLElementDataType
{
	SQL_ELEMENT_RAW,
	SQL_ELEMENT_PREPARED
};

//- The element
struct SQLElementData
{
	SQLElementUnion element;
	SQLElementDataType type;
};

//- For ambigious resultsets
union SQLResultSetUnion
{
	PreparedResultSet* presult;
	ResultSet* qresult;
};

class MySQLConnection;
class SQLOperation
{
public:
	SQLOperation() : conn_(nullptr) { }
	virtual ~SQLOperation() { }

	virtual int call()
	{
		execute();
		return 0;
	}
	virtual bool execute() = 0;
	virtual void setConnection(MySQLConnection* con) { conn_ = con; }

	MySQLConnection* conn_;

private:
	SQLOperation(SQLOperation const& right) = delete;
	SQLOperation& operator=(SQLOperation const& right) = delete;
};


}



