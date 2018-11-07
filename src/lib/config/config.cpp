#include "config.h"
#include "class_module.h"
#include "element_module.h"

using namespace zq;

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, ConfigLib)
};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
    DESTROY_LIB(pm, ConfigLib)
};

#endif


ConfigLib::~ConfigLib() {}

const int ConfigLib::getLibVersion()
{
    return 0;
}

const std::string ConfigLib::getLibName()
{
	return GET_CLASS_NAME(ConfigLib);
}

void ConfigLib::install()
{
    REGISTER_MODULE(libManager_, IClassModule, CClassModule)
    REGISTER_MODULE(libManager_, IElementModule, ElementModule)
}

void ConfigLib::uninstall()
{
    UNREGISTER_MODULE(libManager_, IElementModule, ElementModule)
    UNREGISTER_MODULE(libManager_, IClassModule, CClassModule)
}
