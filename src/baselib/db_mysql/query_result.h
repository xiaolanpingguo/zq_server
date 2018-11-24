#pragma once


#include <future>

namespace zq {

struct Result
{
};

template<typename T>
struct ResultDerive : public Result
{
	ResultDerive(std::vector<T>&& v) :vecResult_(std::move(v)) {}
	std::vector<T> vecResult_;
};

using QueryResult = std::shared_ptr<Result>;
using QueryResultFuture = std::future<QueryResult>;
using QueryResultPromise = std::promise<QueryResult>;

}



