#include "gate_server.h"

namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, GateServerLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, GateServerLib)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int GateServerLib::getLibVersion()
{
    return 0;
}

const std::string GateServerLib::getLibName()
{
	return GET_CLASS_NAME(GateServerLib);
}

void GateServerLib::install()
{

}

void GateServerLib::uninstall()
{

}

}