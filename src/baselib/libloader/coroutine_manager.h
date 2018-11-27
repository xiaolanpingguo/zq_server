#pragma once


#include <list>
#include "interface_header/base/platform.h"

namespace zq {



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

	void newCoroutine();

private:

};



}
