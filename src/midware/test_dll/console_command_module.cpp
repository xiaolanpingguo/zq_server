#include "console_command_module.h"

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
	testDllModule_->reloadDll(name);
	return true;
}

}


