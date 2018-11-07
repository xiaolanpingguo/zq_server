#pragma once

#include "sql_operation.h"
#include "mysql_connection.h"
#include "prepared_statement.h"
#include "query_result.h"

namespace zq {

class SQLQueryHolder
{
	friend class SQLQueryHolderTask;

public:
	SQLQueryHolder() { }
	virtual ~SQLQueryHolder()
	{
		for (size_t i = 0; i < queries_.size(); i++)
		{
			// if the result was never used, free the resources
			// results used already (getresult called) are expected to be deleted
			delete queries_[i].first;
		}
	}

	bool setPreparedQuery(size_t index, PreparedStatement* stmt)
	{
		if (queries_.size() <= index)
		{
			LOG_ERROR << "query index " << index << " out of range (size: " << queries_.size() << " for prepared statement";
			return false;
		}

		queries_[index].first = stmt;
		return true;
	}

	PreparedQueryResult getPreparedResult(size_t index)
	{
		// Don't call to this function if the index is of a prepared statement
		if (index < queries_.size())
		{
			return queries_[index].second;
		}
		else
		{
			return PreparedQueryResult(nullptr);
		}
	}

	void setPreparedResult(size_t index, PreparedResultSet* result)
	{
		if (result && !result->getRowCount())
		{
			delete result;
			result = NULL;
		}

		// store the result in the holder
		if (index < queries_.size())
		{
			queries_[index].second = PreparedQueryResult(result);
		}
	}

	void setSize(size_t size) { queries_.resize(size); }

private:
	std::vector<std::pair<PreparedStatement*, PreparedQueryResult>> queries_;
};


// ªÏ∫œ≤È—Ø
class SQLQueryHolderTask : public SQLOperation
{
public:
	SQLQueryHolderTask(SQLQueryHolder* holder)
		: holder_(holder), isExecuted_(false) { }

	~SQLQueryHolderTask()
	{
		if (!isExecuted_)
		{
			delete holder_;
		}
	}

	bool execute() override
	{
		isExecuted_ = true;

		if (!holder_)
		{
			return false;
		}

		// execute all queries in the holder and pass the results
		for (size_t i = 0; i < holder_->queries_.size(); i++)
		{
			if (PreparedStatement* stmt = holder_->queries_[i].first)
			{
				holder_->setPreparedResult(i, conn_->query(stmt));
			}
		}

		queryResult_.set_value(holder_);

		return true;
	}
	QueryResultHolderFuture getFuture() { return queryResult_.get_future(); }

private:

	SQLQueryHolder * holder_;
	QueryResultHolderPromise queryResult_;
	bool isExecuted_;
};

}



