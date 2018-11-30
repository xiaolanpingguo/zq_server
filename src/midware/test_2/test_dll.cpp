#include "test_dll.h"
#include "test_dll_module.h"
#include "console_command_module.h"

using namespace zq;

//#define ZQ_DYNAMIC_PLUGIN
//#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
	CREATE_LIB(pm, Test2Lib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
	DESTROY_LIB(pm, Test2Lib)
};

//#endif


const int Test2Lib::getLibVersion()
{
    return 0;
}

const std::string Test2Lib::getLibName()
{
	return GET_CLASS_NAME(Test2Lib);
}

void Test2Lib::install()
{
	REGISTER_MODULE(libManager_, ITestDllModule, TestDllModule)
	REGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}

void Test2Lib::uninstall()
{
	UNREGISTER_MODULE(libManager_, ITestDllModule, TestDllModule)
	UNREGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}