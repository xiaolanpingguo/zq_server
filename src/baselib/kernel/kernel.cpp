#include "kernel.h"
#include "kernel_module.h"
#include "event_module.h"
#include "timer_module.h"
#include "class_module.h"
#include "config_module.h"
using namespace zq;


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, KernelLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, KernelLib)
};

#endif


const int KernelLib::getLibVersion()
{
    return 0;
}

const std::string KernelLib::getLibName()
{
	return GET_CLASS_NAME(KernelLib);	     
}

void KernelLib::install()
{
	REGISTER_MODULE(libManager_, IKernelModule, KernelModule)
	REGISTER_MODULE(libManager_, ITimerModule, TimerModule)
	REGISTER_MODULE(libManager_, IClassModule, CClassModule)
	REGISTER_MODULE(libManager_, IConfigModule, ConfigModule)
}

void KernelLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IKernelModule, KernelModule)
	UNREGISTER_MODULE(libManager_, ITimerModule, TimerModule)
	UNREGISTER_MODULE(libManager_, IClassModule, CClassModule)
	UNREGISTER_MODULE(libManager_, IConfigModule, ConfigModule)
}