#include "coroutine_manager.h"	
#include "libmanager.h"

namespace zq {



void executeBody()
{
	LibManager::get_instance().run();
}

void execute()
{
	LibManager::get_instance().run();
}

void CoroutineMgr::init()
{
}

void CoroutineMgr::yieldCo()
{

}

void CoroutineMgr::schedule()
{
	execute();
	while (!pushList_.empty())
	{
		coro_t::coroutine<void>::push_type& push = pushList_.front();
		if (push)
		{
			push();
			break;
		}
		else
		{
			pushList_.pop_front();
		}
	}
}

void CoroutineMgr::newCoroutine(CoroutineFun&& fun)
{
	coro_t::coroutine<void>::push_type sin(std::move(fun));
	pushList_.emplace_back(std::move(sin));
}

}
