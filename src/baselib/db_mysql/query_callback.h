#pragma once


#include "interface_header/base/platform.h"
#include <functional>
#include <queue>
#include "query_result.h"


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
	Construct(obj->_string);
}

template<typename T>
inline void destroyActiveMember(T* obj)
{
	Destroy(obj->_string);
}

template<typename T>
inline void moveFrom(T* to, T&& from)
{
	to->_string = std::move(from._string);
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
		detail::Construct(_string, std::move(result));
	}

	QueryCallback(QueryCallback&& right)
	{
		detail::constructActiveMember(this);
		detail::moveFrom(this, std::move(right));
		_callbacks = std::move(right._callbacks);
	}

	QueryCallback& operator=(QueryCallback&& right)
	{
		if (this != &right)
		{
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
		return withChainingCallback([callback](QueryCallback& /*this*/, QueryResult result) { if (callback) callback(std::move(result)); });
	}

	QueryCallback&& withChainingCallback(std::function<void(QueryCallback&, QueryResult)>&& callback)
	{
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
		auto checkStateAndReturnCompletion = [this]()
		{
			// 弹出元素, 析构
			_callbacks.pop();

			// 如果已经获得了结果，这里valid()会返回false,反之true
			// 但是我想不通为什么这里会返回false,按照代码流程
			// 这个future的值已经被取出来了，所以这里一定会返回true才是啊
			bool hasNext = _string.valid();
			if (_callbacks.empty())
			{
				ASSERT(!hasNext);
				return Completed;
			}

			// 终止调用, 代表已经完成
			if (!hasNext)
				return Completed;

			return NextStep;
		};

		// wait:等待异步操作完成,是阻塞的
		// wait_for是超时等待返回结果，0秒表示立即返回状态
		// std::future_status::ready就表示异步操作已经完成了
		QueryCallbackData& callback = _callbacks.front();
		if (_string.valid() && _string.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			// 这里的_prepared是在之前构造一个查询任务的时候，从promise获得的
			QueryResultFuture f(std::move(_string));

			// 这里的callback._prepared是一个函数(所以敢不敢把名字换下)，是在WithPreparedCallback函数里构造的那个lambda表达式
			// 这里就调用了那个lambda表达式，而这个lambda的实现就是调用用户传过来的函数，
			// 因此这里就表示IO操作已经完成并且回调了用户的函数
			std::function<void(QueryCallback&, QueryResult)> cb(std::move(callback._string));
			cb(*this, f.get());
			return checkStateAndReturnCompletion();
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
	};

private:

	// QueryCallbackData
	struct QueryCallbackData
	{
	public:
		friend class QueryCallback;

		QueryCallbackData(std::function<void(QueryCallback&, QueryResult)>&& callback) : _string(std::move(callback)) { }
		QueryCallbackData(QueryCallbackData&& right)
		{
			detail::constructActiveMember(this);
			detail::moveFrom(this, std::move(right));
		}
		QueryCallbackData& operator=(QueryCallbackData&& right)
		{
			if (this != &right)
			{
				//if (_isPrepared != right._isPrepared)
				//{
				//	detail::destroyActiveMember(this);
				//	_isPrepared = right._isPrepared;
				//	detail::constructActiveMember(this);
				//}
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
		};
	};

	std::queue<QueryCallbackData, std::list<QueryCallbackData>> _callbacks;
};
}



