#include "test_server_module.h"

#include "baselib/message/config_define.hpp"
#include "baselib/message/game_ss.pb.h"

#include "baselib/core/object.h"

using namespace zq;
using namespace std;


bool TestServerModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	logModule_ = libManager_->findModule<ILogModule>();
	httpServerModule_ = libManager_->findModule<IHttpServerModule>();
	httpClientModule_ = libManager_->findModule<IHttpClientModule>();
	eventModule_ = libManager_->findModule<IEventModule>();
	messageDispatcherModule_ = libManager_->findModule<IMessageDispatcherModule>();

	return true;
}

bool TestServerModule::initEnd()
{

	return true;
}

int TestServerModule::onEvent(const Guid& self, const EnEventDefine event, const DataList& arg)
{

	std::cout << "OnEvent EventID: " << event << " self: " << self.nData64 << " argList: " << arg.Int(0) << " " << " " << arg.String(1) << std::endl;

	kernelModule_->setPropertyInt(self, "Hello", arg.Int(0));
	kernelModule_->setPropertyString(self, "Hello", arg.String(1));

	return 0;
}

bool TestServerModule::shut()
{
	return true;
}

bool TestServerModule::run()
{
	test();
	test_http_client();

	return true;
}

void TestServerModule::processMessage(IChannel* channel, const char* data, size_t len, int msgid)
{
	// 50000次请求，花了4539ms，不到5秒
	using namespace std::chrono;

	static uint64 cost_time = 0;
	static uint64 begin_time = 0;
	static uint64 end_time = 0;
	static int count = 0;
	if (begin_time == 0)
	{
		begin_time = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
	}

	count++;

	//if (count % 10000 == 0)
	{
		end_time = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
		cost_time = end_time - begin_time;

		std::cout << "begin_time: " << begin_time << std::endl;
		std::cout << "end_time: " << end_time << std::endl;
		std::cout << "cost_time: " << cost_time << "ms" << std::endl;
		std::cout << "count: " << count << std::endl;
			
		logModule_->info(fmt::format("cost_time: {}", cost_time));
		logModule_->warning(fmt::format("cost_time: {}", cost_time));
		logModule_->error(fmt::format("cost_time: {}", cost_time));
		LOG_INFO << "begin_time: " << begin_time;
		LOG_WARN << "end_time: " << end_time;
		LOG_ERROR << "count: " << count;
	}
}


void TestServerModule::test()
{

}

void TestServerModule::test_http_client()
{
}

