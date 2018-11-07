#pragma once

#include "lib/interface_header/platform.h"
#include "query_callback.h"
#include <algorithm>
#include <vector>

namespace zq {


class QueryCallbackProcessor
{
public:
	QueryCallbackProcessor() = default;
	~QueryCallbackProcessor() = default;

	void addQuery(QueryCallback&& query)
	{
		callbacks_.emplace_back(std::move(query));
	}

	void processReadyQueries()
	{
		if (callbacks_.empty())
		{
			return;
		}

		std::vector<QueryCallback> updateCallbacks{ std::move(callbacks_) };

		// ������Ӧ���Ƴ���Ԫ�ض��ƶ���������β��������һ���ֽ�ĵ�����
		updateCallbacks.erase(std::remove_if(updateCallbacks.begin(), updateCallbacks.end(), [](QueryCallback& callback)
		{
			return callback.invokeIfReady() == QueryCallback::Completed;
		}), updateCallbacks.end());

		// ���������ƶ������������뵽_callbacks��
		callbacks_.insert(callbacks_.end(), std::make_move_iterator(updateCallbacks.begin()), std::make_move_iterator(updateCallbacks.end()));
	}

private:
	QueryCallbackProcessor(QueryCallbackProcessor const&) = delete;
	QueryCallbackProcessor& operator=(QueryCallbackProcessor const&) = delete;

	std::vector<QueryCallback> callbacks_;
};
}



