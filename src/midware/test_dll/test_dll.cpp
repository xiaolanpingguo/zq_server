#include "test_dll.h"
#include "test_dll_module.h"
#include "console_command_module.h"
#include "baselib/libloader/dynlib.h"

using namespace zq;
//#define ZQ_DYNAMIC_PLUGIN
//#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm, DynLib* dynLib)
{
	CREATE_DYN_LIB(pm, TestDll, dynLib)
};

ZQ_EXPORT void dllStop(ILibManager* pm, std::string dynName)
{
	DESTROY_DYN_LIB(pm, dynName)
};

//#endif


const int TestDll::getLibVersion()
{
    return 0;
}

const std::string TestDll::getLibName()
{
	return GET_CLASS_NAME(TestDll);
}

void TestDll::install()
{
	REGISTER_MODULE(libManager_, ITestDllModule, TestDllModule)
}

void TestDll::uninstall()
{
	UNREGISTER_MODULE(libManager_, ITestDllModule, TestDllModule)
}