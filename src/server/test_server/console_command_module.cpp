#include "console_command_module.h"
#include "midware/test_dll/test_dll_module.h"

using namespace zq;


namespace zq{

bool ConsoleCommandModule::init()
{
	testDllModule_ = libManager_->findModule<ITestDllModule>();
	return true;
}

bool ConsoleCommandModule::initEnd()
{	
	
	return true;
}

bool ConsoleCommandModule::run()
{

	return true;
}

bool ConsoleCommandModule::process(const std::string& cmd)
{
	std::string name = cmd.substr(cmd.find_last_of('=') + 1, cmd.length() - cmd.find_last_of('=') - 1);
	auto eqIndex = cmd.find('=');
	auto colonIndex = cmd.find(':');
	auto action = cmd.substr(eqIndex + 1, colonIndex - eqIndex - 1);
	auto argument = cmd.substr(colonIndex + 1, cmd.size() - colonIndex - 1);

	if (action == "reload")
	{
		libManager_->reLoadDynLib(argument);
		testDllModule_ = libManager_->findModule<ITestDllModule>();
	}
	return true;
}

}


