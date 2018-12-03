#include "test_server.h"
#include "test_server_module.h"
#include "console_command_module.h"

namespace zq {


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, TestServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, TestServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int TestServerLib::getLibVersion()
{
    return 0;
}

const std::string TestServerLib::getLibName()
{
	return GET_CLASS_NAME(TestServerLib);
}

void TestServerLib::install()
{
    REGISTER_MODULE(libManager_, ITestServerModule, TestServerModule)
	REGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}

void TestServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, ITestServerModule, TestServerModule)
	UNREGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}

}