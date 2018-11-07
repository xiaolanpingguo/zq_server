#include "kernel.h"
#include "kernel_module.h"
#include "event_module.h"
#include "timer_module.h"
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
	REGISTER_MODULE(libManager_, IEventModule, EventModule)
	REGISTER_MODULE(libManager_, ITimerModule, TimerModule)
}

void KernelLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IEventModule, EventModule)
	UNREGISTER_MODULE(libManager_, IKernelModule, KernelModule)
	UNREGISTER_MODULE(libManager_, ITimerModule, TimerModule)
}