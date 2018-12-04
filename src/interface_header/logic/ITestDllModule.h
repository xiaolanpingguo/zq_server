#pragma once


#include "../base/IModule.h"

namespace zq {



class ITestDllModule : public IModule
{
public:

	virtual void print() = 0;
	virtual void setValue(int value) = 0;
};

}
