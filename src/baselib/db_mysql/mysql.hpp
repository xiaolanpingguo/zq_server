#pragma once


#include <map>
#include <tuple>
#include <limits.h> // INT_MIN
#include "utility.hpp"
#include "query_callback.h"
#include "baselib/base_code/producer_consumer_queue.hpp"

#include <errmsg.h>
#include <mysqld_error.h>


namespace zq {


inline static std::map<std::string, std::string> s_auto_key_map;


class Connection;
class SqlOperation
{
public:
	SqlOperation()
		:conn_(nullptr)
	{ }

	virtual bool execute() = 0;
	virtual void setConnection(Connection* con) { conn_ = con; }
	QueryResultFuture getFuture() const { return resultPromise_->get_future(); }

protected:

	Connection* conn_;
	QueryResultPromise* resultPromise_;
};

template<typename T>
class OperationTask;

class PingTask;

class Connection
{
public:

	Connection(bool async, ProducerConsumerQueue<SqlOperation*>* queue = nullptr)
		:asnyc_(async)
		,stopWork_(false)
	{
		if (asnyc_)
		{
			pcqueue_ = queue;
			workerThread_ = std::make_unique<std::thread>(&Connection::workerThread, this);
		}
	}

	~Connection()
	{
		stopWork_ = true;
		if (workerThread_)
		{
			workerThread_->join();
		}
		disconnect();
	}

	void workerThread()
	{
		while (1)
		{
			if (stopWork_)
			{
				return;
			}

			SqlOperation* operation = nullptr;
			pcqueue_->waitAndPop(operation);
			operation->setConnection(this);
			operation->execute();
			delete operation;
		}
	}

	template<typename... Args>
	bool connect(Args&&... args) 
	{
		int timeout = -1;
		args_ = std::tuple_cat(get_tp(timeout, std::forward<Args>(args)...), std::make_tuple(0, nullptr, 0));
		return connectMysql() == 0;
	}

	uint32 connectMysql()
	{
		if (con_ != nullptr)
		{
			mysql_close(con_);
		}

		con_ = mysql_init(nullptr);
		if (con_ == nullptr)
		{
			return false;
		}

		std::get<0>(args_) = con_;
		int timeout = -1;
		if (timeout > 0)
		{
			if (mysql_options(con_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout) != 0)
			{
				return CR_UNKNOWN_ERROR;
			}
		}

		char value = 1;
		mysql_options(con_, MYSQL_OPT_RECONNECT, &value);
		mysql_options(con_, MYSQL_SET_CHARSET_NAME, "utf8");

		if (std::apply(&mysql_real_connect, args_) != nullptr)
		{
			return 0;
		}
		else
		{
			return mysql_errno(con_);
		}
	}

	bool ping() 
	{
		return mysql_ping(con_) == 0;
	}

	template<typename... Args>
	bool disconnect(Args&&... args)
	{
		if (con_ != nullptr) {
			mysql_close(con_);
			con_ = nullptr;
		}

		return true;
	}

	template<typename T, typename... Args >
	constexpr bool create_datatable(Args&&... args) 
	{
		//            std::string droptb = "DROP TABLE IF EXISTS ";
		//            droptb += iguana::get_name<T>();
		//            if (mysql_query(con_, droptb.data())) {
		//                return false;
		//            }

		std::string sql = generate_createtb_sql<T>(std::forward<Args>(args)...);
		if (mysql_query(con_, sql.data())) 
		{
			LOG_ERROR << "create_datatable error: sql: " << sql;
			return false;
		}

		return true;
	}

	template<typename T>
	constexpr int insert(const std::string& sql, const T& t)
	{
		bool success = false;
		do 
		{
			stmt_ = mysql_stmt_init(con_);
			if (!stmt_)
				break;

			if (mysql_stmt_prepare(stmt_, sql.c_str(), (int)sql.size()))
			{
				break;
			}

			auto guard = guard_statment(stmt_);

			if (stmt_execute(t) < 0)
				break;

			success = true;
		} while (0);

		if (!success)
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "mysql_stmt_bind_result error:  " << lErrno << " errmsg: " << mysql_stmt_error(stmt_);

			// 如果返回成功，代表是重连成功，这里再进行一次查询
			if (handleMySQLErrno(lErrno))
				return insert(sql, t);
			else
				return 0;
		}

		return 1;
	}

	template<typename T>
	constexpr int insertBatch(const std::string& sql, const std::vector<T>& t)
	{
		stmt_ = mysql_stmt_init(con_);
		if (!stmt_)
			return INT_MIN;

		if (mysql_stmt_prepare(stmt_, sql.c_str(), (int)sql.size()))
		{
			return INT_MIN;
		}

		auto guard = guard_statment(stmt_);

		//transaction
		bool b = begin();
		if (!b)
			return INT_MIN;

		for (auto& item : t)
		{
			int r = stmt_execute(item);
			if (r == INT_MIN)
			{
				rollback();
				return INT_MIN;
			}
		}
		b = commit();

		return b ? (int)t.size() : INT_MIN;
	}

	template<typename T>
	constexpr int update(const std::string& sql, const T& t)
	{
		return insert(sql, t);
	}

	template<typename T>
	constexpr int updateBatch(const std::string& sql, const std::vector<T>& t)
	{
		return insertBatch(sql, t);
	}

	bool delete_records(std::string_view sql) 
	{
		if (mysql_query(con_, sql.data())) 
		{
			LOG_ERROR << "close statment error code: " << mysql_error(con_);
			return false;
		}

		return true;
	}

	template<typename T>
	constexpr std::enable_if_t<iguana::is_reflection_v<T>, std::vector<T>> query(std::string_view sql)
	{
		constexpr auto SIZE = iguana::get_value<T>();

		std::vector<T> v;
		T t{};
		bool success = false;
		do 
		{
			stmt_ = mysql_stmt_init(con_);
			if (!stmt_)
			{
				break;
			}

			if (mysql_stmt_prepare(stmt_, sql.data(), (unsigned long)sql.size()))
			{
				break;
			}

			auto guard = guard_statment(stmt_);

			std::array<MYSQL_BIND, SIZE> param_binds = {};
			std::map<size_t, std::vector<char>> mp;

			iguana::for_each(t, [&](auto item, auto i)
			{
				constexpr auto Idx = decltype(i)::value;
				using U = std::remove_reference_t<decltype(std::declval<T>().*item)>;
				if constexpr (std::is_arithmetic_v<U>)
				{
					param_binds[Idx].buffer_type = (enum_field_types)type_to_id(identity<U>{});
					param_binds[Idx].buffer = &(t.*item);
				}
				else if constexpr (std::is_same_v<std::string, U>)
				{
					param_binds[Idx].buffer_type = MYSQL_TYPE_STRING;
					std::vector<char> tmp(65536, 0);
					mp.emplace(decltype(i)::value, tmp);
					param_binds[Idx].buffer = &(mp.rbegin()->second[0]);
					param_binds[Idx].buffer_length = (unsigned long)tmp.size();
				}
				else if constexpr (is_char_array_v<U>)
				{
					param_binds[Idx].buffer_type = MYSQL_TYPE_VAR_STRING;
					std::vector<char> tmp(sizeof(U), 0);
					mp.emplace(decltype(i)::value, tmp);
					param_binds[Idx].buffer = &(mp.rbegin()->second[0]);
					param_binds[Idx].buffer_length = (unsigned long)sizeof(U);
				}
			});

			if (mysql_stmt_bind_result(stmt_, &param_binds[0]))
			{
				break;
			}

			if (mysql_stmt_execute(stmt_))
			{									   
				break;
			}

			while (mysql_stmt_fetch(stmt_) == 0)
			{
				using TP = decltype(iguana::get(std::declval<T>()));

				iguana::for_each(t, [&mp, &t](auto item, auto i)
				{
					using U = std::remove_reference_t<decltype(std::declval<T>().*item)>;
					if constexpr (std::is_same_v<std::string, U>)
					{
						auto& vec = mp[decltype(i)::value];
						t.*item = std::string(&vec[0], strlen(vec.data()));
					}
					else if constexpr (is_char_array_v<U>) {
						auto& vec = mp[decltype(i)::value];
						memcpy(t.*item, vec.data(), vec.size());
					}
				});

				v.push_back(std::move(t));
			}

			success = true;
		} while (0);

		if (!success)
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "mysql_stmt_bind_result error:  " << lErrno << " errmsg: " << mysql_stmt_error(stmt_);

			// 如果返回成功，代表是重连成功，这里再进行一次查询
			if (handleMySQLErrno(lErrno))
				return query<T>(sql);
			else
				return {};
		}
		else
		{
			return v;
		}
	}

	template<typename T>
	QueryResult _async_query(std::string_view sql)
	{
		return QueryResult(new ResultDerive<T>(std::move(query<T>(sql))));
	}

	//for tuple and string with args...
	template<typename T, typename Arg, typename... Args>
	constexpr std::enable_if_t<!iguana::is_reflection_v<T>, std::vector<T>> query(const Arg& s, Args&&... args) 
	{
		static_assert(iguana::is_tuple<T>::value);
		constexpr auto SIZE = std::tuple_size_v<T>;

		std::string sql = s;
		constexpr auto Args_Size = sizeof...(Args);
		if (Args_Size != 0) 
		{
			if (Args_Size != std::count(sql.begin(), sql.end(), '?'))
			{
				has_error_ = true;
				return {};
			}

			sql = get_sql(sql, std::forward<Args>(args)...);
		}

		std::vector<T> v;
		T tp{};
		bool success = false;
		do
		{
			stmt_ = mysql_stmt_init(con_);
			if (!stmt_)
			{
				break;
			}

			if (mysql_stmt_prepare(stmt_, sql.c_str(), (int)sql.size()))
			{
				break;
			}

			auto guard = guard_statment(stmt_);

			std::array<MYSQL_BIND, result_size<T>::value> param_binds = {};
			std::list<std::vector<char>> mp;

			size_t index = 0;
			iguana::for_each(tp, [&param_binds, &mp, &index](auto& item, auto I)
			{
				using U = std::remove_reference_t<decltype(item)>;
				if constexpr (std::is_arithmetic_v<U>)
				{
					param_binds[index].buffer_type = (enum_field_types)type_to_id(identity<U>{});
					param_binds[index].buffer = &item;
					index++;
				}
				else if constexpr (std::is_same_v<std::string, U>)
				{
					std::vector<char> tmp(65536, 0);
					mp.emplace_back(std::move(tmp));
					param_binds[index].buffer_type = MYSQL_TYPE_STRING;
					param_binds[index].buffer = &(mp.back()[0]);
					param_binds[index].buffer_length = 65536;
					index++;
				}
				else if constexpr (iguana::is_reflection_v<U>)
				{
					iguana::for_each(item, [&param_binds, &mp, &item, &index](auto& ele, auto i)
					{
						using U = std::remove_reference_t<decltype(std::declval<U>().*ele)>;
						if constexpr (std::is_arithmetic_v<U>)
						{
							param_binds[index].buffer_type = (enum_field_types)type_to_id(identity<U>{});
							param_binds[index].buffer = &(item.*ele);
						}
						else if constexpr (std::is_same_v<std::string, U>)
						{
							std::vector<char> tmp(65536, 0);
							mp.emplace_back(std::move(tmp));
							param_binds[index].buffer_type = MYSQL_TYPE_STRING;
							param_binds[index].buffer = &(mp.back()[0]);
							param_binds[index].buffer_length = 65536;
						}
						else if constexpr (is_char_array_v<U>)
						{
							std::vector<char> tmp(sizeof(U), 0);
							mp.emplace_back(std::move(tmp));
							param_binds[index].buffer_type = MYSQL_TYPE_VAR_STRING;
							param_binds[index].buffer = &(mp.back()[0]);
							param_binds[index].buffer_length = (unsigned long)sizeof(U);
						}
						index++;
					});
				}
				else if constexpr (is_char_array_v<U>)
				{
					param_binds[index].buffer_type = MYSQL_TYPE_VAR_STRING;
					std::vector<char> tmp(sizeof(U), 0);
					mp.emplace_back(std::move(tmp));
					param_binds[index].buffer = &(mp.back()[0]);
					param_binds[index].buffer_length = (unsigned long)sizeof(U);
					index++;
				}
				else {
					std::cout << typeid(U).name() << std::endl;
				}
			}, std::make_index_sequence<SIZE>{});

			if (mysql_stmt_bind_result(stmt_, &param_binds[0]))
			{
				break;
			}

			if (mysql_stmt_execute(stmt_))
			{
				break;
			}

			while (mysql_stmt_fetch(stmt_) == 0)
			{
				auto it = mp.begin();
				iguana::for_each(tp, [&mp, &it](auto& item, auto i)
				{
					using U = std::remove_reference_t<decltype(item)>;
					if constexpr (std::is_same_v<std::string, U>)
					{
						item = std::string(&(*it)[0], strlen((*it).data()));
						it++;
					}
					else if constexpr (is_char_array_v<U>)
					{
						memcpy(item, &(*it)[0], sizeof(U));
					}
					else if constexpr (iguana::is_reflection_v<U>)
					{
						iguana::for_each(item, [&mp, &it, &item](auto ele, auto i)
						{
							using V = std::remove_reference_t<decltype(std::declval<U>().*ele)>;
							if constexpr (std::is_same_v<std::string, V>)
							{
								item.*ele = std::string(&(*it)[0], strlen((*it).data()));
								it++;
							}
							else if constexpr (is_char_array_v<V>)
							{
								memcpy(item.*ele, &(*it)[0], sizeof(V));
							}
						});
					}
				}, std::make_index_sequence<SIZE>{});

				v.push_back(std::move(tp));

				success = true;
			}
		} while (0);

		if (!success)
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "occur error, sql: " << sql << ", error: " << lErrno << " errmsg: " << mysql_stmt_error(stmt_);

			// 如果返回成功，代表是重连成功，这里再进行一次查询
			if (handleMySQLErrno(lErrno))
				return query<T>(s, std::forward<Args>(args)...);
			else
				return {};
		}
		else
		{
			return v;
		}
	}

	bool has_error() 
	{
		return has_error_;
	}

	bool execute(const std::string& sql) 
	{
		if (mysql_query(con_, sql.data()) != 0) 
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "mysql_query error, sql: " << sql << ", error: " << lErrno;

			// 如果返回成功，代表是重连成功，这里再进行一次查询
			if (handleMySQLErrno(lErrno))
				return execute(sql);

			return false;
		}

		return true;
	}

	//transaction
	bool begin() 
	{
		if (mysql_query(con_, "BEGIN")) 
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "begin error, error: " << lErrno;
			return false;
		}

		return true;
	}

	bool commit() 
	{
		if (mysql_query(con_, "COMMIT")) 
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "commit error, error: " << lErrno;
			return false;
		}

		return true;
	}

	bool rollback()
	{
		if (mysql_query(con_, "ROLLBACK"))
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "rollback error, error: " << lErrno;
			return false;
		}

		return true;
	}

private:
	template<typename T, typename... Args >
	std::string generate_createtb_sql(Args&&... args) 
	{
		const auto type_name_arr = get_type_names<T>();
		constexpr auto name = iguana::get_name<T>();
		std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + name.data() + "(";
		auto arr = iguana::get_array<T>();
		constexpr auto SIZE = sizeof... (Args);
		s_auto_key_map[name.data()] = "";

		//auto_increment_key and key can't exist at the same time
		using U = std::tuple<std::decay_t <Args>...>;
		if constexpr (SIZE > 0) 
		{
			//using U = std::tuple<std::decay_t <Args>...>;//the code can't compile in vs
			static_assert(!(iguana::has_type<primary_key, U>::value && iguana::has_type<primary_auto_incr_key, U>::value),
				"should only one key");
		}
		auto tp = sort_tuple(std::make_tuple(std::forward<Args>(args)...));
		const size_t arr_size = arr.size();
		for (size_t i = 0; i < arr_size; ++i) 
		{
			auto field_name = arr[i];
			bool has_add_field = false;
			for_each0(tp, [&sql, &i, &has_add_field, field_name, type_name_arr, name, this](auto item) 
			{
				if constexpr (std::is_same_v<decltype(item), key_not_null>)
				{
					if (item.fields.find(field_name.data()) == item.fields.end())
						return;
				}
				else 
				{
					if (item.fields != field_name.data())
						return;
				}

				if constexpr (std::is_same_v<decltype(item), key_not_null>) 
				{
					if (!has_add_field)
					{
						append(sql, field_name.data(), " ", type_name_arr[i]);
					}
					append(sql, " NOT NULL");
					has_add_field = true;
				}
				else if constexpr (std::is_same_v<decltype(item), primary_key>)
				{
					if (!has_add_field) 
					{
						append(sql, field_name.data(), " ", type_name_arr[i]);
					}
					append(sql, " PRIMARY KEY");
					has_add_field = true;
				}
				else if constexpr (std::is_same_v<decltype(item), primary_auto_incr_key>)
				{
					if (!has_add_field) 
					{
						append(sql, field_name.data(), " ", type_name_arr[i]);
					}
					append(sql, " AUTO_INCREMENT");
					append(sql, " PRIMARY KEY");
					s_auto_key_map[name.data()] = item.fields;
					has_add_field = true;
				}
				else if constexpr (std::is_same_v<decltype(item), unique_key>)
				{
					if (!has_add_field) 
					{
						append(sql, field_name.data(), " ", type_name_arr[i]);
					}

					append(sql, ", UNIQUE(", item.fields, ")");
					has_add_field = true;
				}
				else
				{
					append(sql, field_name.data(), " ", type_name_arr[i]);
				}
			}, std::make_index_sequence<SIZE>{});

			if (!has_add_field) 
			{
				append(sql, field_name.data(), " ", type_name_arr[i]);
			}

			if (i < arr_size - 1)
				sql += ", ";
		}

		sql += ")";
		return sql;
	}

	template<typename T>
	constexpr void set_param_bind(std::vector<MYSQL_BIND>& param_binds, T&& value) 
	{
		MYSQL_BIND param = {};

		using U = std::remove_const_t<std::remove_reference_t<T>>;
		if constexpr(std::is_arithmetic_v<U>) 
		{
			param.buffer_type = (enum_field_types)type_to_id(identity<U>{});
			param.buffer = const_cast<void*>(static_cast<const void*>(&value));
		}
		else if constexpr(std::is_same_v<std::string, U>) 
		{
			param.buffer_type = MYSQL_TYPE_STRING;
			param.buffer = (void*)(value.c_str());
			param.buffer_length = (unsigned long)value.size();
		}
		else if constexpr(std::is_same_v<const char*, U> || is_char_array_v<U>) 
		{
			param.buffer_type = MYSQL_TYPE_STRING;
			param.buffer = (void*)(value);
			param.buffer_length = (unsigned long)strlen(value);
		}

		param_binds.push_back(param);
	}

	template<typename T>
	int stmt_execute(const T& t)
	{
		std::vector<MYSQL_BIND> param_binds;
		auto it = s_auto_key_map.find(iguana::get_name<T>().data());
		std::string auto_key = (it == s_auto_key_map.end()) ? "" : it->second;

		iguana::for_each(t, [&t, &param_binds, &auto_key, this](const auto& v, auto i)
		{
			/*if (!auto_key.empty() && auto_key == iguana::get_name<T>(decltype(i)::value).data())
				return;*/

			set_param_bind(param_binds, t.*v);
		});

		bool success = false;
		do
		{
			if (mysql_stmt_bind_param(stmt_, &param_binds[0]))
			{
				break;
			}

			if (mysql_stmt_execute(stmt_))
			{				
				break;
			}

			success = true;

		} while (0);

		if (!success)
		{
			uint32 lErrno = mysql_errno(con_);
			LOG_ERROR << "occur error,error: " << lErrno << " errmsg: " << mysql_stmt_error(stmt_);

			// 如果返回成功，代表是重连成功，这里再进行一次查询
			if (handleMySQLErrno(lErrno))
				return stmt_execute(t);

			return INT_MIN;
		}

		int count = (int)mysql_stmt_affected_rows(stmt_);
		if (count == 0)
		{
			return INT_MIN;
		}

		return count;
	}

	bool handleMySQLErrno(uint32 errNo, uint8 attempts = 5)
	{
		switch (errNo)
		{
		case CR_SERVER_GONE_ERROR:
		case CR_SERVER_LOST:
		case CR_INVALID_CONN_HANDLE:
		case CR_SERVER_LOST_EXTENDED:
		case CR_CONN_HOST_ERROR:
		{
			LOG_ERROR << "Lost the connection, Attempting to reconnect to the MySQL server...";

			uint32 const lErrno = connectMysql();
			if (lErrno == 0)
			{
				LOG_INFO << "mysql Successfully reconnected, db: " << std::get<4>(args_);
				return true;
			}

			if ((--attempts) == 0)
			{
				// 彻底连不上了，直接退出吧
				LOG_ERROR<< "Failed to reconnect to the MySQL server, "
					"terminating the server to prevent data corruption!";

				std::this_thread::sleep_for(std::chrono::seconds(10));
				std::abort();
			}
			else
			{
				// 重连可能会导致2006的错误，这里再试几次，睡眠三秒是防止递归爆栈
				std::this_thread::sleep_for(std::chrono::seconds(3)); 
				return handleMySQLErrno(lErrno, attempts); 
			}
		}

		case ER_LOCK_DEADLOCK:
			return false;   
		// Query related errors - skip query
		case ER_WRONG_VALUE_COUNT:
		case ER_DUP_ENTRY:
			return false;

			// Outdated table or database structure - terminate core
		case ER_BAD_FIELD_ERROR:
		case ER_NO_SUCH_TABLE:
			LOG_ERROR << "Your database structure is error.";
			std::this_thread::sleep_for(std::chrono::seconds(10));
			std::abort();
			return false;
		case ER_PARSE_ERROR:
			LOG_ERROR << "Error while parsing SQL. Core fix required.";
			std::this_thread::sleep_for(std::chrono::seconds(10));
			std::abort();
			return false;
		default:
			LOG_ERROR << "Unhandled MySQL errno %u. Unexpected behaviour possible, errNo:" << errNo;
			return false;
		}
	}


	struct guard_statment 
	{
		guard_statment(MYSQL_STMT* stmt) :stmt_(stmt) {}
		MYSQL_STMT* stmt_ = nullptr;
		int status_ = 0;
		~guard_statment() 
		{
			if (stmt_ != nullptr)
				status_ = mysql_stmt_close(stmt_);

			if (status_)
				LOG_ERROR << "close statment error code: " << status_;
		}
	};

	template<typename... Args>
	auto get_tp(int& timeout, Args&&... args) 
	{
		auto tp = std::make_tuple(con_, std::forward<Args>(args)...);
		if constexpr (sizeof...(Args) == 5) 
		{
			auto[c, s1, s2, s3, s4, i] = tp;
			timeout = i;
			return std::make_tuple(c, s1, s2, s3, s4);
		}
		else
			return tp;
	}

private:

	std::unique_ptr<std::thread> workerThread_;
	ProducerConsumerQueue<SqlOperation*>* pcqueue_;

	bool asnyc_;
	std::atomic<bool> stopWork_;

	MYSQL* con_ = nullptr;
	MYSQL_STMT* stmt_ = nullptr;
	bool has_error_ = false;

	// mysql:ip:user:pwd:db:port:unix_socket:client_flag
	std::tuple<MYSQL*, const char*, const char*, const char*, const char*, int, const char*, int> args_;
};



template <typename T>
class QueryTask : public SqlOperation
{
public:
	QueryTask(std::string_view sql)
		:sql_(sql)
	{
		resultPromise_ = new QueryResultPromise;
	}

	virtual bool execute()
	{
		QueryResult result = conn_->_async_query<T>(sql_);
		resultPromise_->set_value(result);
		return true;
	}

private:
	std::string sql_;
};

class PingTask : public SqlOperation
{
public:
	virtual bool execute()
	{
		conn_->ping();
		return true;
	}
};

}

