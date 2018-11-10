#include "network.h"


namespace zq{

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
}

void NetworkLib::uninstall()
{
}

}