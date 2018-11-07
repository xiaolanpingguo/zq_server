#include "component.h"
#include "component_module.h"
using namespace zq;


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, ComponentLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, ComponentLib)
};

#endif


const int ComponentLib::getLibVersion()
{
    return 0;
}

const std::string ComponentLib::getLibName()
{
	return GET_CLASS_NAME(ComponentLib);
}

void ComponentLib::install()
{
    REGISTER_MODULE(libManager_, IComponentModule, ComponentModule)
}

void ComponentLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IComponentModule, ComponentModule)
}