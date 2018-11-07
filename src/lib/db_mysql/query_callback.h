#pragma once

#include "lib/interface_header/platform.h"
#include "database_env_fwd.h"
#include <functional>
#include <list>
#include <queue>
#include <utility>

namespace zq {

namespace detail{

template<typename T, typename... Args>
inline void Construct(T& t, Args&&... args)
{
	new (&t) T(std::forward<Args>(args)...);
}

template<typename T>
inline void Destroy(T& t)
{
	t.~T();
}

template<typename T>
inline void constructActiveMember(T* obj)
{
	if (!obj->_isPrepared)
		Construct(obj->_string);
	else
		Construct(obj->_prepared);
}

template<typename T>
inline void destroyActiveMember(T* obj)
{
	if (!obj->_isPrepared)
		Destroy(obj->_string);
	else
		Destroy(obj->_prepared);
}

template<typename T>
inline void moveFrom(T* to, T&& from)
{
	ASSERT(to->_isPrepared == from._isPrepared);

	if (!to->_isPrepared)
		to->_string = std::move(from._string);
	else
		to->_prepared = std::move(from._prepared);
}
}


class QueryCallback
{
	friend class QueryCallbackProcessor;
public:

	enum Status
	{
		NotReady,
		NextStep,
		Completed
	};

	explicit QueryCallback(QueryResultFuture&& result)
	{
		// 这里的构造函数,移动构造,和析构函数我都没理解为什么会专门写一个模版函数来构造
		// 直接new T(std::move(result))不行吗, 为什么需要用到placement new呢
		_isPrepared = false;
		detail::Construct(_string, std::move(result));
	}

	explicit QueryCallback(PreparedQueryResultFuture&& result)
	{
		_isPrepared = true;
		detail::Construct(_prepared, std::move(result));
	}

	QueryCallback(QueryCallback&& right)
	{
		_isPrepared = right._isPrepared;
		detail::constructActiveMember(this);
		detail::moveFrom(this, std::move(right));
		_callbacks = std::move(right._callbacks);
	}

	QueryCallback& operator=(QueryCallback&& right)
	{
		if (this != &right)
		{
			if (_isPrepared != right._isPrepared)
			{
				detail::destroyActiveMember(this);
				_isPrepared = right._isPrepared;
				detail::constructActiveMember(this);
			}
			detail::moveFrom(this, std::move(right));
			_callbacks = std::move(right._callbacks);
		}
		return *this;
	}

	~QueryCallback()
	{
		detail::destroyActiveMember(this);
	}

	QueryCallback&& withCallback(std::function<void(QueryResult)>&& callback)
	{
		return withChainingCallback([callback](QueryCallback& /*this*/, QueryResult result) { callback(std::move(result)); });
	}

	QueryCallback&& withPreparedCallback(std::function<void(PreparedQueryResult)>&& callback)
	{
		// callback是用户传过来的参数，也是最终会回调给用户的
		// 但是这里又构造了一个lambda,这个lambda捕获了用户传过来的这个函数,它的实现就是
		// 一句代码，回调用户的这个函数
		// 之后就把这个lambda存到容器里，等待IO的完成后调用这个lambda,从而调回用户的函数
		// 但是不理解的是为什么不直接用容器直接存储callback这个对象，还要
		// 用一个lambda来构造QueryCallbackData装进容器呢 
		return withChainingPreparedCallback([callback](QueryCallback& /*this*/, PreparedQueryResult result) { callback(std::move(result)); });
	}


	QueryCallback&& withChainingCallback(std::function<void(QueryCallback&, QueryResult)>&& callback)
	{
		// 只针对于基本查询
		ASSERT(!_callbacks.empty() || !_isPrepared, "Attempted to set callback function for string query on a prepared async query");
		_callbacks.emplace(std::move(callback));
		return std::move(*this);
	}

	QueryCallback&& withChainingPreparedCallback(std::function<void(QueryCallback&, PreparedQueryResult)>&& callback)
	{
		// 只针对于prepare查询
		ASSERT(!_callbacks.empty() || _isPrepared, "Attempted to set callback function for prepared query on a string async query");
		_callbacks.emplace(std::move(callback));
		return std::move(*this);
	}

	// Moves std::future from next to this object
	void setNextQuery(QueryCallback&& next)
	{
		detail::moveFrom(this, std::move(next));
	}

	Status invokeIfReady()
	{
		// 先获取一个对象
		QueryCallbackData& callback = _callbacks.front();
		auto checkStateAndReturnCompletion = [this]()
		{
			// 弹出元素, 析构
			_callbacks.pop();

			// 如果已经获得了结果，这里valid()会返回false,反之true
			// 但是我想不通为什么这里会返回false,按照代码流程
			// 这个future的值已经被取出来了，所以这里一定会返回true才是啊
			bool hasNext = !_isPrepared ? _string.valid() : _prepared.valid();
			if (_callbacks.empty())
			{
				ASSERT(!hasNext);
				return Completed;
			}

			// abort chain
			// 代表已经完成
			if (!hasNext)
				return Completed;

			// 这个断言肯定是成立的啊
			// 但是目前还不明白为什么会走到这一步
			ASSERT(_isPrepared == _callbacks.front()._isPrepared);
			return NextStep;
		};

		if (!_isPrepared)
		{
			if (_string.valid() && _string.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				QueryResultFuture f(std::move(_string));
				std::function<void(QueryCallback&, QueryResult)> cb(std::move(callback._string));
				cb(*this, f.get());
				return checkStateAndReturnCompletion();
			}
		}
		else
		{
			// wait:等待异步操作完成,是阻塞的
			// wait_for是超时等待返回结果，0秒表示立即返回状态
			// std::future_status::ready就表示异步操作已经完成了
			if (_prepared.valid() && _prepared.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				// 这里的_prepared是在之前构造一个查询任务的时候，从promise获得的
				PreparedQueryResultFuture f(std::move(_prepared));

				// 这里的callback._prepared是一个函数(所以敢不敢把名字换下)，是在WithPreparedCallback函数里构造的那个lambda表达式
				// 这里就调用了那个lambda表达式，而这个lambda的实现就是调用用户传过来的函数，
				// 因此这里就表示IO操作已经完成并且回调了用户的函数
				std::function<void(QueryCallback&, PreparedQueryResult)> cb(std::move(callback._prepared));
				cb(*this, f.get());

				// 检测相关状态并且返回
				return checkStateAndReturnCompletion();
			}
		}

		return NotReady;
	}

private:
	QueryCallback(QueryCallback const& right) = delete;
	QueryCallback& operator=(QueryCallback const& right) = delete;

	template<typename T> friend void detail::constructActiveMember(T* obj);
	template<typename T> friend void detail::destroyActiveMember(T* obj);
	template<typename T> friend void detail::moveFrom(T* to, T&& from);

	union
	{
		QueryResultFuture _string;
		PreparedQueryResultFuture _prepared;
	};
	bool _isPrepared;

private:

	// QueryCallbackData
	struct QueryCallbackData
	{
	public:
		friend class QueryCallback;

		QueryCallbackData(std::function<void(QueryCallback&, QueryResult)>&& callback) : _string(std::move(callback)), _isPrepared(false) { }
		QueryCallbackData(std::function<void(QueryCallback&, PreparedQueryResult)>&& callback) : _prepared(std::move(callback)), _isPrepared(true) { }
		QueryCallbackData(QueryCallbackData&& right)
		{
			_isPrepared = right._isPrepared;
			detail::constructActiveMember(this);
			detail::moveFrom(this, std::move(right));
		}
		QueryCallbackData& operator=(QueryCallbackData&& right)
		{
			if (this != &right)
			{
				if (_isPrepared != right._isPrepared)
				{
					detail::destroyActiveMember(this);
					_isPrepared = right._isPrepared;
					detail::constructActiveMember(this);
				}
				detail::moveFrom(this, std::move(right));
			}
			return *this;
		}
		~QueryCallbackData() { detail::destroyActiveMember(this); }

	private:
		QueryCallbackData(QueryCallbackData const&) = delete;
		QueryCallbackData& operator=(QueryCallbackData const&) = delete;

		template<typename T> friend void detail::constructActiveMember(T* obj);
		template<typename T> friend void detail::destroyActiveMember(T* obj);
		template<typename T> friend void detail::moveFrom(T* to, T&& from);

		union
		{
			std::function<void(QueryCallback&, QueryResult)> _string;
			std::function<void(QueryCallback&, PreparedQueryResult)> _prepared;
		};
		bool _isPrepared;
	};

	std::queue<QueryCallbackData, std::list<QueryCallbackData>> _callbacks;
};
}



