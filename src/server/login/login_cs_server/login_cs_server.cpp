#include "login_cs_server.h"
#include "login_cs_module.h"
#include "console_command_module.h"


namespace zq{


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, LoginCSServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, LoginCSServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int LoginCSServerLib::getLibVersion()
{
    return 0;
}

const std::string LoginCSServerLib::getLibName()
{
	return GET_CLASS_NAME(LoginCSServerLib);
}

void LoginCSServerLib::install()
{
    REGISTER_MODULE(libManager_, ILoginCSModule, LoginCSModule)
	REGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}

void LoginCSServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, ILoginCSModule, LoginCSModule)
	UNREGISTER_MODULE(libManager_, IConsoleCommandModule, ConsoleCommandModule)
}

}