#pragma once


#include <string_view>
#include <vector>
#include <functional>
#include <chrono>
#include <array>
#include "utility.hpp"
#include "mysql.hpp"
#include "query_callback_processor.h"
#include "baselib/base_code/producer_consumer_queue.hpp"

namespace zq{


class ConnectionPool
{
	enum InternalIndex
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

public:
	ConnectionPool(int asyncThreads = 1)
		:asyncThreads_(asyncThreads)
	{
		pcqueue_ = new ProducerConsumerQueue<SqlOperation*>();
	}

    ~ConnectionPool()
	{
		for (const auto& vec : _connections)
		{
			for (const auto& conn: vec)
			{
				conn->disconnect();
			}
		}
    }

	bool run()
	{
		cbProcessor_.processReadyQueries();
		return true;
	}
	
    template <typename... Args>
    bool open(Args&&... args)
	{
		{
			std::unique_ptr<Connection> conn = std::make_unique<Connection>(false);
			if (!conn->connect(std::forward<Args>(args)...))
			{
				return false;
			}
			_connections[IDX_SYNCH].emplace_back(std::move(conn));
		}

		{
			for (int i = 0; i < asyncThreads_; ++i)
			{
				std::unique_ptr<Connection> conn = std::make_unique<Connection>(true, pcqueue_);
				if (!conn->connect(std::forward<Args>(args)...))
				{
					return false;
				}

				_connections[IDX_ASYNC].emplace_back(std::move(conn));
			}
		}

		return true;;
    }

    template<typename T, typename... Args>
    bool create_datatable(Args&&... args)
	{	
		return _connections[IDX_SYNCH][0]->template create_datatable<T>(std::forward<Args>(args)...);;
    }

    template<typename T, typename... Args>
    int insert(const T& t,Args&&... args)
	{
		auto name = iguana::get_name<T>().data();
		std::string sql = s_auto_key_map[name].empty() ? generate_insert_sql<T>(false) : generate_auto_insert_sql<T>(s_auto_key_map, false);
		return _connections[IDX_SYNCH][0]->insert(sql, t);
    }

	template<typename T, typename... Args>
	void async_insert(const T& t, Args&&... args)
	{
		auto name = iguana::get_name<T>().data();
		std::string sql = s_auto_key_map[name].empty() ? generate_insert_sql<T>(false) : generate_auto_insert_sql<T>(s_auto_key_map, false);
		queueTask<T>(sql);
	}

    template<typename T, typename... Args>
    int insertBatch(const std::vector<T>& t, Args&&... args)
	{
		auto name = iguana::get_name<T>().data();
		std::string sql = s_auto_key_map[name].empty() ? generate_insert_sql<T>(false) : generate_auto_insert_sql<T>(s_auto_key_map, false);
		return _connections[IDX_SYNCH][0]->insertBatch(sql, t);
    }

    template<typename T, typename... Args>
    int update(const T& t, Args&&... args) 
	{
		std::string sql = generate_insert_sql<T>(true);
        return _connections[IDX_SYNCH][0]->update(sql, t);
    }

	template<typename T, typename... Args>
	void async_update(const T& t, Args&&... args)
	{
		auto name = iguana::get_name<T>().data();
		std::string sql = s_auto_key_map[name].empty() ? generate_insert_sql<T>(false) : generate_auto_insert_sql<T>(s_auto_key_map, false);
		queueTask<T>(sql);
	}

    template<typename T, typename... Args>
    int updateBatch(const std::vector<T>& t, Args&&... args)
	{
		std::string sql = generate_insert_sql<T>(true);
        return _connections[IDX_SYNCH][0]->updateBatch(t);
    }

    template<typename T, typename... Args>
    bool delete_records(Args&&... where_conditon)
	{
		auto sql = generate_delete_sql<T>(std::forward<Args>(where_conditon)...);
        return _connections[IDX_SYNCH][0]->delete_records(sql);
    }

	template<typename T, typename... Args>
	void async_delete_records(Args&&... where_conditon)
	{
		auto sql = generate_delete_sql<T>(std::forward<Args>(where_conditon)...);
		queueTask<T>(sql);
	}

	template<typename Pair, typename U>
	bool delete_records(Pair pair, std::string_view oper, U&& val)
	{
		auto sql = build_condition(pair, oper, std::forward<U>(val));
		using T = typename field_attribute<decltype(pair.second)>::type;
		return delete_records<T>(sql);
	}

	//restriction, all the args are string, the first is the where condition, rest are append conditions
	template<typename T, typename... Args>
	std::vector<T> query(Args&&... args)
	{
		std::string sql = generate_query_sql<T>(args...);
		return _connections[IDX_SYNCH][0]->template query<T>(sql);
	}

	template<typename T, typename Fun, typename... Args>
	void async_query(Fun&& f, Args&&... args)
	{
		std::string sql = generate_query_sql<T>(args...);
		queueTask<T>(sql, std::move(f));
	}

    //support member variable, such as: query(FID(simple::id), "<", 5)
    template<typename Pair, typename U>
    auto query(Pair pair, std::string_view oper, U&& val)
	{
        auto sql = build_condition(pair, oper, std::forward<U>(val));
        using T = typename field_attribute<decltype(pair.second)>::type;
        return query<T>(sql);
    }

    bool execute(const std::string& sql)
	{
        return _connections[IDX_SYNCH][0]->execute(sql);
    }

    //transaction
    bool begin()
	{
        return _connections[IDX_SYNCH][0]->begin();
    }

    bool commit()
	{
        return _connections[IDX_SYNCH][0]->commit();
    }

    bool rollback()
	{
        return _connections[IDX_SYNCH][0]->rollback();
    }

	bool ping() 
	{
		return _connections[IDX_SYNCH][0]->ping();
	}

	bool has_error() 
	{
		return _connections[IDX_SYNCH][0]->has_error();
	}

private:

	void keepAlive()
	{
		for (auto& connection : _connections[IDX_SYNCH])
		{
			connection->ping();
		}

		auto const count = _connections[IDX_ASYNC].size();
		for (uint8 i = 0; i < count; ++i)
		{
			SqlOperation* task = new PingTask();
			QueryResultFuture result = task->getFuture();
			pcqueue_->push(task);
			cbProcessor_.addQuery(QueryCallback(std::move(result)).withCallback(std::move(nullptr)));
		}
	}

	template<typename T, typename Fun>
	void queueTask(std::string_view sql, Fun&& f = nullptr)
	{
		SqlOperation* task = new QueryTask<T>(sql);
		QueryResultFuture result = task->getFuture();
		pcqueue_->push(task);
		cbProcessor_.addQuery(QueryCallback(std::move(result)).withCallback(std::move(f)));
	}

	template<typename Pair, typename U>
	auto build_condition(Pair pair, std::string_view oper, U&& val)
	{
		std::string sql = "";
		using T = typename field_attribute<decltype(pair.second)>::type;
		using V = std::remove_const_t<std::remove_reference_t<U>>;

		//if field type is numeric, return type of val is numeric, to string; val is string, no change;
		//if field type is string, return type of val is numeric, to string and add ''; val is string, add '';
		using return_type = typename field_attribute<decltype(pair.second)>::return_type;

		if constexpr(std::is_arithmetic_v<return_type>&&std::is_arithmetic_v<V>)
		{
			append(sql, pair.first, oper, std::to_string(std::forward<U>(val)));
		}
		else if constexpr(!std::is_arithmetic_v<return_type>)
		{
			if constexpr(std::is_arithmetic_v<V>)
				append(sql, pair.first, oper, to_str(std::to_string(std::forward<U>(val))));
			else
				append(sql, pair.first, oper, to_str(std::forward<U>(val)));
		}
		else
		{
			append(sql, pair.first, oper, std::forward<U>(val));
		}

		return sql;
	}

#define HAS_MEMBER(member)\
template<typename T, typename... Args>\
struct has_##member\
{\
	private:\
	template<typename U> static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
	template<typename U> static std::false_type Check(...);\
	public:\
	enum{value = std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};

    HAS_MEMBER(before)
    HAS_MEMBER(after)

#define WRAPER(func)\
template<typename... AP, typename... Args>\
auto wraper##_##func(Args&&... args){\
    using result_type = decltype(std::declval<decltype(this)>()->func(std::declval<Args>()...));\
    bool r = true;\
    std::tuple<AP...> tp{};\
    for_each_l(tp, [&r, &args...](auto& item){\
        if(!r)\
            return;\
        if constexpr (has_before<decltype(item)>::value)\
        r = item.before(std::forward<Args>(args)...);\
    }, std::make_index_sequence<sizeof...(AP)>{});\
    if(!r)\
        return result_type{};\
    auto lambda = [this, &args...]{ return this->func(std::forward<Args>(args)...); };\
    result_type result = std::invoke(lambda);\
    for_each_r(tp, [&r, &result, &args...](auto& item){\
        if(!r)\
            return;\
        if constexpr (has_after<decltype(item), result_type>::value)\
        r = item.after(result, std::forward<Args>(args)...);\
    }, std::make_index_sequence<sizeof...(AP)>{});\
    return result;\
}

	template <typename... Args, typename F, std::size_t... Idx>
	constexpr void for_each_l(std::tuple<Args...>& t, F&& f, std::index_sequence<Idx...>)
	{
		(std::forward<F>(f)(std::get<Idx>(t)), ...);
	}

	template <typename... Args, typename F, std::size_t... Idx>
	constexpr void for_each_r(std::tuple<Args...>& t, F&& f, std::index_sequence<Idx...>)
	{
		constexpr auto size = sizeof...(Idx);
		(std::forward<F>(f)(std::get<size - Idx - 1>(t)), ...);
	}

public:

    WRAPER(connect);
    WRAPER(execute);

    void update_operate_time()
	{
        latest_tm_ = std::chrono::system_clock::now();
    }

    auto get_latest_operate_time()
	{
        return latest_tm_;
    }
private:

	ProducerConsumerQueue<SqlOperation*>* pcqueue_;
	QueryCallbackProcessor cbProcessor_;
	std::array<std::vector<std::unique_ptr<Connection>>, IDX_SIZE> _connections;
	int asyncThreads_; 

	std::chrono::system_clock::time_point latest_tm_ = std::chrono::system_clock::now();
};


}

