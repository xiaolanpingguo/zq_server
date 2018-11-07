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
		// ����Ĺ��캯��,�ƶ�����,�����������Ҷ�û���Ϊʲô��ר��дһ��ģ�溯��������
		// ֱ��new T(std::move(result))������, Ϊʲô��Ҫ�õ�placement new��
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
		// callback���û��������Ĳ�����Ҳ�����ջ�ص����û���
		// ���������ֹ�����һ��lambda,���lambda�������û����������������,����ʵ�־���
		// һ����룬�ص��û����������
		// ֮��Ͱ����lambda�浽������ȴ�IO����ɺ�������lambda,�Ӷ������û��ĺ���
		// ���ǲ�������Ϊʲô��ֱ��������ֱ�Ӵ洢callback������󣬻�Ҫ
		// ��һ��lambda������QueryCallbackDataװ�������� 
		return withChainingPreparedCallback([callback](QueryCallback& /*this*/, PreparedQueryResult result) { callback(std::move(result)); });
	}


	QueryCallback&& withChainingCallback(std::function<void(QueryCallback&, QueryResult)>&& callback)
	{
		// ֻ����ڻ�����ѯ
		ASSERT(!_callbacks.empty() || !_isPrepared, "Attempted to set callback function for string query on a prepared async query");
		_callbacks.emplace(std::move(callback));
		return std::move(*this);
	}

	QueryCallback&& withChainingPreparedCallback(std::function<void(QueryCallback&, PreparedQueryResult)>&& callback)
	{
		// ֻ�����prepare��ѯ
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
		// �Ȼ�ȡһ������
		QueryCallbackData& callback = _callbacks.front();
		auto checkStateAndReturnCompletion = [this]()
		{
			// ����Ԫ��, ����
			_callbacks.pop();

			// ����Ѿ�����˽��������valid()�᷵��false,��֮true
			// �������벻ͨΪʲô����᷵��false,���մ�������
			// ���future��ֵ�Ѿ���ȡ�����ˣ���������һ���᷵��true���ǰ�
			bool hasNext = !_isPrepared ? _string.valid() : _prepared.valid();
			if (_callbacks.empty())
			{
				ASSERT(!hasNext);
				return Completed;
			}

			// abort chain
			// �����Ѿ����
			if (!hasNext)
				return Completed;

			// ������Կ϶��ǳ����İ�
			// ����Ŀǰ��������Ϊʲô���ߵ���һ��
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
			// wait:�ȴ��첽�������,��������
			// wait_for�ǳ�ʱ�ȴ����ؽ����0���ʾ��������״̬
			// std::future_status::ready�ͱ�ʾ�첽�����Ѿ������
			if (_prepared.valid() && _prepared.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				// �����_prepared����֮ǰ����һ����ѯ�����ʱ�򣬴�promise��õ�
				PreparedQueryResultFuture f(std::move(_prepared));

				// �����callback._prepared��һ������(���ԸҲ��Ұ����ֻ���)������WithPreparedCallback�����ﹹ����Ǹ�lambda���ʽ
				// ����͵������Ǹ�lambda���ʽ�������lambda��ʵ�־��ǵ����û��������ĺ�����
				// �������ͱ�ʾIO�����Ѿ���ɲ��һص����û��ĺ���
				std::function<void(QueryCallback&, PreparedQueryResult)> cb(std::move(callback._prepared));
				cb(*this, f.get());

				// ������״̬���ҷ���
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



