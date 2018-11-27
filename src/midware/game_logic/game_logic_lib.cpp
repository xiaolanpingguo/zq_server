#include "game_logic_lib.h"
#include "game_logic_module.h"
#include "addons_module/addons_module.h"


namespace zq {

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
	CREATE_LIB(pm, GameLogicLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
	DESTROY_LIB(pm, GameLogicLib)
};

#endif


const int GameLogicLib::getLibVersion()
{
	return 0;
}

const std::string GameLogicLib::getLibName()
{
	return GET_CLASS_NAME(GameLogicLib);
}

void GameLogicLib::install()
{
	REGISTER_MODULE(libManager_, IGameLogicModule, GameLogicModule)
	REGISTER_MODULE(libManager_, IAddonsModule, AddonsModule)
}

void GameLogicLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IGameLogicModule, GameLogicModule)
	UNREGISTER_MODULE(libManager_, IAddonsModule, AddonsModule)
}

}