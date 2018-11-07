#pragma once

#include <future>
#include <memory>
#include <functional>

// mysql
using MYSQL = struct st_mysql;
using MYSQL_RES = struct st_mysql_res;
using MYSQL_FIELD = struct st_mysql_field;
using MYSQL_BIND = struct st_mysql_bind;
using MYSQL_STMT = struct st_mysql_stmt;

namespace zq {

class Field;

class ResultSet;
using QueryResult = std::shared_ptr<ResultSet>;
using QueryResultFuture = std::future<QueryResult>;
using QueryResultPromise = std::promise<QueryResult>;
using QueryResultCb = std::function<void(QueryResult)>;

class PreparedStatement;
class PreparedResultSet;
using PreparedQueryResult = std::shared_ptr<PreparedResultSet>;
using PreparedQueryResultFuture = std::future<PreparedQueryResult>;
using PreparedQueryResultPromise = std::promise<PreparedQueryResult>;
using PreparedQueryResultCb = std::function<void(PreparedQueryResult)>;

class QueryCallback;
class Transaction;
using SQLTransaction = std::shared_ptr<Transaction>;

class SQLQueryHolder;
using QueryResultHolderFuture = std::future<SQLQueryHolder*>;
using QueryResultHolderPromise = std::promise<SQLQueryHolder*>;

}




