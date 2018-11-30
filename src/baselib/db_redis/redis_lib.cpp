#include "redis_lib.h"
#include "redis_module.h"


namespace zq{

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
	CREATE_LIB(pm, RedisLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
	DESTROY_LIB(pm, RedisLib)
};

#endif


const int RedisLib::getLibVersion()
{
	return 0;
}

const std::string RedisLib::getLibName()
{
	return GET_CLASS_NAME(RedisLib);
}

void RedisLib::install()
{
	REGISTER_MODULE(libManager_, IRedisModule, RedisModule)
}

void RedisLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IRedisModule, RedisModule)
}

}