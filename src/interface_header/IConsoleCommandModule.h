#pragma once


#include "IModule.h"

namespace zq {


class IConsoleCommandModule : public IModule
{
public:

	virtual bool process(const std::string& cmd) { return true; }

};

}
