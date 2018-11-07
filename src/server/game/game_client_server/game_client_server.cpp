#include "game_client_server.h"
#include "game_to_world_module.h"


namespace zq{


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, GameClientServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, GameClientServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int GameClientServerLib::getLibVersion()
{
    return 0;
}

const std::string GameClientServerLib::getLibName()
{
	return GET_CLASS_NAME(GameClientServerLib);
}

void GameClientServerLib::install()
{
	REGISTER_MODULE(libManager_, IGameToWorldModule, GameToWorldModule)
}

void GameClientServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IGameToWorldModule, GameToWorldModule)
}

}