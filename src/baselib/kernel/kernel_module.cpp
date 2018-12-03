#include "kernel_module.h"
#include "baselib/core/object.h"
#include "interface_header/base/uuid.h"
#include "baselib/message/config_define.hpp"
#include <random>

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <windows.h>
#endif

namespace zq{

KernelModule::KernelModule(ILibManager* p)
{
    libManager_ = p;
}

KernelModule::~KernelModule()
{
}

void KernelModule::initRandom()
{
    vecRandom_.clear();

	static constexpr int random_max = 100000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1.0f);

    for (int i = 0; i < random_max; i++)
    {
        vecRandom_.push_back((float) dis(gen));
    }
}

bool KernelModule::init()
{
    classModule_ = libManager_->findModule<IClassModule>();
	randomEngine_.seed(getMSTime());
	initRandom();

    return true;
}

bool KernelModule::initEnd()
{
	using namespace std::placeholders;

	return true;
}

bool KernelModule::shut()
{
	vecRandom_.clear();

    return true;
}

bool KernelModule::run()
{
    return true;
}

uuid KernelModule::gen_uuid()
{
	static uint64 tv = (uint64)(time(nullptr));
	uint64 now = (uint64)(time(nullptr));

	static uint16 lastNum = 0;

	if (now != tv)
	{
		tv = now;
		lastNum = 0;
	}

	// 32位     16位                16位
	// 时间戳	serverid的后16位	每次自增的数
	// 需要保证在一秒类不能连续调用超过65536次
	int32 server_id = libManager_->getServerID();
	if (server_id <= 0)
	{
		static uint32 rnd = 0;
		if (rnd == 0)
		{
			rnd = (uint32)(randomEngine_() << 16);
		}

		assert(lastNum < 65535 && "gen_uuid(): overflow!");
		return (tv << 32) | rnd | lastNum++;
	}
	else
	{
		static uint32 sections = server_id << 16;
		assert(lastNum < 65535 && "gen_uuid(): overflow!");
		return (tv << 32) | sections | lastNum++;
	}
}



}