#include "test_dll_module.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#endif

using namespace zq;

TestDllModule::TestDllModule(ILibManager* p)
{
    libManager_ = p;
}

TestDllModule::~TestDllModule()
{
}

bool zq::TestDllModule::init()
{
	print();
	return true;
}

bool TestDllModule::initEnd()
{
	return true;
}

bool TestDllModule::run()
{
    return true;
}

bool TestDllModule::shut()
{
	return true;
}

void zq::TestDllModule::print()
{
	std::cout << 1234567 << std::endl;
}

void zq::TestDllModule::reloadDll(std::string name)
{
	libManager_->reLoadDynLib(name);
}
