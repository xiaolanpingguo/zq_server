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

		// 将所有应该移除的元素都移动到了容器尾部并返回一个分界的迭代器
		updateCallbacks.erase(std::remove_if(updateCallbacks.begin(), updateCallbacks.end(), [](QueryCallback& callback)
		{
			return callback.invokeIfReady() == QueryCallback::Completed;
		}), updateCallbacks.end());

		// 构造两个移动迭代器，插入到_callbacks中
		callbacks_.insert(callbacks_.end(), std::make_move_iterator(updateCallbacks.begin()), std::make_move_iterator(updateCallbacks.end()));
	}

private:
	QueryCallbackProcessor(QueryCallbackProcessor const&) = delete;
	QueryCallbackProcessor& operator=(QueryCallbackProcessor const&) = delete;

	std::vector<QueryCallback> callbacks_;
};
}



