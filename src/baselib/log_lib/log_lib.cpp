#include "log_lib.h"
#include "log_module.h"

using namespace zq;

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, LogLib)
};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, LogLib)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int LogLib::getLibVersion()
{
    return 0;
}

const std::string LogLib::getLibName()
{
	return GET_CLASS_NAME(LogLib);
}

void LogLib::install()
{
    REGISTER_MODULE(libManager_, ILogModule, LogModule)
}

void LogLib::uninstall()
{
    UNREGISTER_MODULE(libManager_, ILogModule, LogModule)
}