#pragma once

#include "baselib/interface_header/IConsoleCommandModule.h"

namespace zq {

class ConsoleCommandModule : public IConsoleCommandModule
{

public:
	ConsoleCommandModule(ILibManager* p)
	{
		libManager_ = p;
	}

	bool init() override;
	bool initEnd() override;
	bool run() override;

public:

	bool process(const std::string& cmd);

private:


};

}

