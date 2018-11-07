#include "data_agent_lib.h"
#include "data_agent_module.h"
using namespace zq;

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
	CREATE_LIB(pm, DataAgentLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
	DESTROY_LIB(pm, DataAgentLib)
};

#endif


const int DataAgentLib::getLibVersion()
{
	return 0;
}

const std::string DataAgentLib::getLibName()
{
	return GET_CLASS_NAME(DataAgentLib);
}

void DataAgentLib::install()
{
	REGISTER_MODULE(libManager_, IDataAgentModule, DataAgentModule)
}

void DataAgentLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IDataAgentModule, DataAgentModule)
}