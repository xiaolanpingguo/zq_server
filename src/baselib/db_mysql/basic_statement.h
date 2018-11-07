#pragma once

#include "database_env_fwd.h"
#include "sql_operation.h"
#include "mysql_connection.h"
#include "query_result.h"

namespace zq {


// »ù±¾²éÑ¯
class BasicStatementTask : public SQLOperation
{
public:
	BasicStatementTask(const char* sql, bool async = false)
		:queryResult_(nullptr),
		sql_(sql),
		hasResult_(async)
	{
		if (async)
		{
			queryResult_ = new QueryResultPromise();
		}
	}
	~BasicStatementTask()
	{
		if (hasResult_ && queryResult_ != nullptr)
		{
			delete queryResult_;
		}
	}

	bool execute() override
	{
		if (hasResult_)
		{
			ResultSet* result = conn_->query(sql_.c_str());
			if (!result || !result->getRowCount() || !result->nextRow())
			{
				delete result;
				queryResult_->set_value(QueryResult(NULL));
				return false;
			}

			queryResult_->set_value(QueryResult(result));
			return true;
		}

		return conn_->execute(sql_.c_str());
	}

	QueryResultFuture getFuture() const { return queryResult_->get_future(); }

private:
	std::string sql_;
	bool hasResult_;
	QueryResultPromise* queryResult_;
};

}




