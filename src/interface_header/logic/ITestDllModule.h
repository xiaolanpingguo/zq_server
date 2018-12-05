#pragma once


#include "../base/IModule.h"

namespace zq {

	class response;
	class request;

class ITestDllModule : public IModule
{
public:

	virtual void print(request& req, response& res) = 0;
};

}
