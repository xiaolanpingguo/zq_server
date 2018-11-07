#include "world_ss_server.h"
#include "world_ss_module.h"
using namespace zq;


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, WorldSSServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, WorldSSServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int WorldSSServerLib::getLibVersion()
{
    return 0;
}

const std::string WorldSSServerLib::getLibName()
{
	return GET_CLASS_NAME(WorldSSServerLib);
}

void WorldSSServerLib::install()
{
    REGISTER_MODULE(libManager_, IWorldSSModule, WorldSSModule)
}

void WorldSSServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IWorldSSModule, WorldSSModule)
}