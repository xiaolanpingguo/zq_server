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

}

void CoroutineMgr::newCoroutine()
{
}

}
