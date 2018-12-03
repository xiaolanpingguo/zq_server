#include "test_dll_module.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#endif

using namespace zq;

TestDllModule::TestDllModule(ILibManager* p)
{
    libManager_ = p;
	v = 0;
}

TestDllModule::~TestDllModule()
{
}

bool zq::TestDllModule::init()
{
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
	std::cout << 7654321 << std::endl;
	std::cout << v << std::endl;
}

void zq::TestDllModule::setValue(int value)
{
	v = value;
}
