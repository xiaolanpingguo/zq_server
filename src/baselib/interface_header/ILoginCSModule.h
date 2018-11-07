#pragma once

#include "IModule.h"

namespace zq {


class ILoginCSModule : public IModule
{
public:

	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}