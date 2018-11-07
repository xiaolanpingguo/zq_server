#include "message_lib.h"
#include "messge_dispatcher_module.h"
#include "proto_json_module.h"
using namespace zq;


#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
	CREATE_LIB(pm, MessageLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{
	DESTROY_LIB(pm, MessageLib)
};

#endif


const int MessageLib::getLibVersion()
{
	return 0;
}

const std::string MessageLib::getLibName()
{
	return GET_CLASS_NAME(MessageLib);
}

void MessageLib::install()
{
	REGISTER_MODULE(libManager_, IMessageDispatcherModule, MessageDispatcherModule)
	REGISTER_MODULE(libManager_, IProtoJsonModule, ProtoJsonModule)
}

void MessageLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IMessageDispatcherModule, MessageDispatcherModule)
	UNREGISTER_MODULE(libManager_, IProtoJsonModule, ProtoJsonModule)
}