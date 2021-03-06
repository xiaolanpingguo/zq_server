#include "baselib/libloader/libmanager.h"
#include "midware/data_agent/data_agent_lib.h"
#include "midware/game_logic/game_logic_lib.h"
#include "game_cs_server.h"
#include "../game_client_server/game_client_server.h"
using namespace std;
using namespace zq;

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libloader_d.lib")
#pragma comment(lib, "cryptography_d.lib")
#pragma comment(lib, "data_agent_d.lib")
#pragma comment(lib, "zlib_d.lib")
#pragma comment(lib, "g3dlib_d.lib")
#pragma comment(lib, "game_client_server_d.lib")
#pragma comment(lib, "game_logic_d.lib")
#endif

int main(int argc, char* argv[])
{
	LibManager::get_instance().launch<ST_GAME_SERVER, GameCSServerLib, GameClientServerLib, DataAgentLib, GameLogicLib>(argc, argv);
	LibManager::get_instance().loop();

	return 0;
}
