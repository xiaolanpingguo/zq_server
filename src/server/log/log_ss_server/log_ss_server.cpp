#include "log_ss_server.h"


namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, LogSSServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, LogSSServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int LogSSServerLib::getLibVersion()
{
    return 0;
}

const std::string LogSSServerLib::getLibName()
{
	return GET_CLASS_NAME(LogSSServerLib);
}

void LogSSServerLib::install()
{

}

void LogSSServerLib::uninstall()
{

}

}