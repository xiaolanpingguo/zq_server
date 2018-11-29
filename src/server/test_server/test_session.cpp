#include "test_session.h"
#include "baselib/libloader/libmanager.h"
#include "baselib/base_code/format.h"
#include <iostream>
#include "interface_header/base/IRedislModule.h"


using namespace std;

namespace zq {



void foo(TestSession* session)
{
	IRedisModule* redis_module = LibManager::get_instance().findModule<IRedisModule>();

	std::string out;
	static int i = 0;
	redis_module->getClientBySuitConsistent()->GET(std::to_string(i++), out);

	cout << "i: " << i << endl;
}

TestSession::TestSession(tcp_t::socket&& socket)
	:InternalSocket(std::move(socket))
{

}
TestSession::~TestSession()
{

}

void TestSession::start()
{
	std::cout << "client has connect success, ip:" << getIp() <<" port:" << getPort() << std::endl;
}

void TestSession::onClose()
{
	std::cout << "client has disconnect, ip:" << getIp() << " port:" << getPort() << " id: " << getServerId() << std::endl;
}

void TestSession::readHandler()
{
#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
	go std::bind(&foo, this);
#endif
}



}
