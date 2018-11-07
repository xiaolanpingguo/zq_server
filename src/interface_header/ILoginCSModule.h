#pragma once

#include "IModule.h"

namespace zq {


class ILoginCSModule : public IModule
{
public:

	virtual bool createAccount(const std::string& account_name, const std::string& pwd) = 0;

	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}