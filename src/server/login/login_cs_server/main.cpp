#include "baselib/libloader/libmanager.h"
#include "midware/data_agent/data_agent_lib.h"
#include "login_cs_server.h"
#include "../login_client_server/login_client_server.h"
using namespace std;
using namespace zq;

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libloader_d.lib")
#pragma comment(lib, "cryptography_d.lib")
#pragma comment(lib, "data_agent_d.lib")
#pragma comment(lib, "login_client_server_d.lib")
#endif

int main(int argc, char* argv[])
{
	LibManager::get_instance().launch<ST_LOGIN_SERVER, LoginCSServerLib, LoginClientServerLib, DataAgentLib>(argc, argv);
	LibManager::get_instance().loop();

	return 0;
}
