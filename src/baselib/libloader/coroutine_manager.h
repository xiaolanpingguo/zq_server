#pragma once


#include <list>
#include "interface_header/base/platform.h"
#include <boost/coroutine2/all.hpp>

namespace zq {



namespace coro_t = boost::coroutines2;
using CoroutineFun = std::function<void(coro_t::coroutine<void>::pull_type&)>;
struct Coroutine;
class CoroutineMgr
{
public:

	static CoroutineMgr& get_instance()
	{
		static CoroutineMgr ins;
		return ins;
	}

	void init();

	void yieldCo();

	void schedule();

	void newCoroutine(CoroutineFun&& fun);

private:

	std::list<coro_t::coroutine<void>::push_type> pushList_;
};



}
