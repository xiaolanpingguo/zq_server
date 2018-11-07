#pragma once


#include "IModule.h"

namespace zq {


class ILoginLogicModule : public IModule
{
public:

	virtual bool _createAccount(const std::string& account_name, const std::string& pwd) { return true; };
};
}
