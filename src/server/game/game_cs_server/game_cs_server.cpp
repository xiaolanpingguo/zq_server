#include "game_cs_server.h"
#include "game_cs_module.h"

namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, GameCSServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, GameCSServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int GameCSServerLib::getLibVersion()
{
    return 0;
}

const std::string GameCSServerLib::getLibName()
{
	return GET_CLASS_NAME(GameCSServerLib);
}

void GameCSServerLib::install()
{
	REGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
}

void GameCSServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
}

}