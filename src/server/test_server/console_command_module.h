#pragma once


#include "interface_header/base/IConsoleCommandModule.h"
#include "interface_header/logic/ITestDllModule.h"

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

	ITestDllModule* testDllModule_;
};

}
