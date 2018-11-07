#include "baselib/libloader/libmanager.h"
#include "log_ss_server.h"
using namespace std;
using namespace zq;

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libloader_d.lib")
#endif

int main(int argc, char* argv[])
{
	LibManager::get_instance().launch<ST_LOG_SERVER, LogSSServerLib>(argc, argv);
	LibManager::get_instance().loop();

	return 0;
}
