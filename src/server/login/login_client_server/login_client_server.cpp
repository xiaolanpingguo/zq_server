#include "login_client_server.h"
#include "login_to_world_module.h"


namespace zq{


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, LoginClientServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, LoginClientServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int LoginClientServerLib::getLibVersion()
{
    return 0;
}

const std::string LoginClientServerLib::getLibName()
{
	return GET_CLASS_NAME(LoginClientServerLib);
}

void LoginClientServerLib::install()
{
	REGISTER_MODULE(libManager_, ILoginToWorldModule, LoginToWorldModule)
}

void LoginClientServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, ILoginToWorldModule, LoginToWorldModule)
}

}