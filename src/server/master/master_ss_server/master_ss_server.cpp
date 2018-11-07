#include "master_ss_server.h"


namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, MasterSSServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, MasterSSServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int MasterSSServerLib::getLibVersion()
{
    return 0;
}

const std::string MasterSSServerLib::getLibName()
{
	return GET_CLASS_NAME(MasterSSServerLib);
}

void MasterSSServerLib::install()
{

}

void MasterSSServerLib::uninstall()
{

}

}