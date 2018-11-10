#pragma once

#include "interface_header/base/platform.h"
#include "sql_operation.h"
#include "query_result.h"
#include "mysql_connection.h"
#include <vector>

#ifdef __APPLE__
#undef TYPE_BOOL
#endif

namespace zq {


union PreparedStatementDataUnion
{
	bool boolean;
	uint8 ui8;
	int8 i8;
	uint16 ui16;
	int16 i16;
	uint32 ui32;
	int32 i32;
	uint64 ui64;
	int64 i64;
	float f;
	double d;
};

enum PreparedStatementValueType
{
	TYPE_BOOL,
	TYPE_UI8,
	TYPE_UI16,
	TYPE_UI32,
	TYPE_UI64,
	TYPE_I8,
	TYPE_I16,
	TYPE_I32,
	TYPE_I64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BINARY,
	TYPE_NULL
};

struct PreparedStatementData
{
	PreparedStatementDataUnion data;
	PreparedStatementValueType type;
	std::vector<uint8> binary;
};

class MySQLPreparedStatement;
class PreparedStatement
{
	friend class PreparedStatementTask;
	friend class MySQLPreparedStatement;
	friend class MySQLConnection;

public:
	explicit PreparedStatement(uint32 index);
	~PreparedStatement();

	void setBool(const uint8 index, const bool value);
	void setUInt8(const uint8 index, const uint8 value);
	void setUInt16(const uint8 index, const uint16 value);
	void setUInt32(const uint8 index, const uint32 value);
	void setUInt64(const uint8 index, const uint64 value);
	void setInt8(const uint8 index, const int8 value);
	void setInt16(const uint8 index, const int16 value);
	void setInt32(const uint8 index, const int32 value);
	void setInt64(const uint8 index, const int64 value);
	void setFloat(const uint8 index, const float value);
	void setDouble(const uint8 index, const double value);
	void setString(const uint8 index, const std::string& value);
	void setBinary(const uint8 index, const std::vector<uint8>& value);
	void setNull(const uint8 index);

protected:
	void bindParameters();

protected:
	MySQLPreparedStatement* m_stmt;
	uint32 m_index;
	std::vector<PreparedStatementData> statement_data;    //- Buffer of parameters, not tied to MySQL in any way yet

	PreparedStatement(PreparedStatement const& right) = delete;
	PreparedStatement& operator=(PreparedStatement const& right) = delete;
};


class MySQLPreparedStatement
{
	friend class MySQLConnection;
	friend class PreparedStatement;

public:
	MySQLPreparedStatement(MYSQL_STMT* stmt);
	~MySQLPreparedStatement();

	void setNull(const uint8 index);
	void setBool(const uint8 index, const bool value);
	void setUInt8(const uint8 index, const uint8 value);
	void setUInt16(const uint8 index, const uint16 value);
	void setUInt32(const uint8 index, const uint32 value);
	void setUInt64(const uint8 index, const uint64 value);
	void setInt8(const uint8 index, const int8 value);
	void setInt16(const uint8 index, const int16 value);
	void setInt32(const uint8 index, const int32 value);
	void setInt64(const uint8 index, const int64 value);
	void setFloat(const uint8 index, const float value);
	void setDouble(const uint8 index, const double value);
	void setBinary(const uint8 index, const std::vector<uint8>& value, bool isString);

protected:
	MYSQL_STMT * getSTMT() { return m_Mstmt; }
	MYSQL_BIND* getBind() { return m_bind; }
	PreparedStatement* m_stmt;
	void clearParameters();
	void checkValidIndex(uint8 index);
	std::string getQueryString(std::string const& sqlPattern) const;

private:
	MYSQL_STMT * m_Mstmt;
	uint32 m_paramCount;
	std::vector<bool> m_paramsSet;
	MYSQL_BIND* m_bind;

	MySQLPreparedStatement(MySQLPreparedStatement const& right) = delete;
	MySQLPreparedStatement& operator=(MySQLPreparedStatement const& right) = delete;
};



// 预查询的异步任务
class PreparedStatementTask : public SQLOperation
{
public:
	PreparedStatementTask(PreparedStatement* stmt, bool async = false)
		:stmt_(stmt), queryResult_(nullptr)
	{
		hasResult_ = async;
		if (async)
		{
			queryResult_ = new PreparedQueryResultPromise();
		}
	}
	~PreparedStatementTask()
	{
		delete stmt_;
		if (hasResult_ && queryResult_ != nullptr)
		{
			delete queryResult_;
		}
	}

	bool execute() override
	{
		if (hasResult_)
		{
			PreparedResultSet* result = conn_->query(stmt_);
			if (!result || !result->getRowCount())
			{
				delete result;
				queryResult_->set_value(PreparedQueryResult(nullptr));
				return false;
			}

			queryResult_->set_value(PreparedQueryResult(result));

			return true;
		}

		return conn_->execute(stmt_);
	}
	PreparedQueryResultFuture getFuture() { return queryResult_->get_future(); }

protected:
	PreparedStatement * stmt_;
	bool hasResult_;
	PreparedQueryResultPromise* queryResult_;
};


}


