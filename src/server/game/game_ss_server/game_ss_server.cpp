#include "game_server.h"
#include "game_cs_module.h"
#include "game_ss_module.h"

namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, GameServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, GameServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int GameServerLib::getLibVersion()
{
    return 0;
}

const std::string GameServerLib::getLibName()
{
	return GET_CLASS_NAME(GameServerLib);
}

void GameServerLib::install()
{
    REGISTER_MODULE(libManager_, IGameSSModule, GameSSModule)
	REGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
}

void GameServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IGameSSModule, GameSSModule)
	UNREGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
}

}