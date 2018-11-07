#include "network.h"
#include "net_ss_module.h"
using namespace zq;

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, NetworkLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{													  
    DESTROY_LIB(pm, NetworkLib)
};								  

#endif


const int NetworkLib::getLibVersion()
{
    return 0;
}

const std::string NetworkLib::getLibName()
{
	return GET_CLASS_NAME(NetworkLib);
}

void NetworkLib::install()
{
    REGISTER_MODULE(libManager_, INetSSModule, NetSSModule)
}

void NetworkLib::uninstall()
{
    UNREGISTER_MODULE(libManager_, INetSSModule, NetSSModule)
}