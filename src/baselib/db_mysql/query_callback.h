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
		// ����Ĺ��캯��,�ƶ�����,�����������Ҷ�û���Ϊʲô��ר��дһ��ģ�溯��������
		// ֱ��new T(std::move(result))������, Ϊʲô��Ҫ�õ�placement new��
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
			// ����Ԫ��, ����
			_callbacks.pop();

			// ����Ѿ�����˽��������valid()�᷵��false,��֮true
			// �������벻ͨΪʲô����᷵��false,���մ�������
			// ���future��ֵ�Ѿ���ȡ�����ˣ���������һ���᷵��true���ǰ�
			bool hasNext = _string.valid();
			if (_callbacks.empty())
			{
				ASSERT(!hasNext);
				return Completed;
			}

			// ��ֹ����, �����Ѿ����
			if (!hasNext)
				return Completed;

			return NextStep;
		};

		// wait:�ȴ��첽�������,��������
		// wait_for�ǳ�ʱ�ȴ����ؽ����0���ʾ��������״̬
		// std::future_status::ready�ͱ�ʾ�첽�����Ѿ������
		QueryCallbackData& callback = _callbacks.front();
		if (_string.valid() && _string.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			// �����_prepared����֮ǰ����һ����ѯ�����ʱ�򣬴�promise��õ�
			QueryResultFuture f(std::move(_string));

			// �����callback._prepared��һ������(���ԸҲ��Ұ����ֻ���)������WithPreparedCallback�����ﹹ����Ǹ�lambda���ʽ
			// ����͵������Ǹ�lambda���ʽ�������lambda��ʵ�־��ǵ����û��������ĺ�����
			// �������ͱ�ʾIO�����Ѿ���ɲ��һص����û��ĺ���
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



