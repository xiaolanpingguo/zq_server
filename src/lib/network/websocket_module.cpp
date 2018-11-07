#include "websocket_module.h"
using namespace zq;

WebSocketModule::WebSocketModule(ILibManager* p)
{
	libManager_ = p;
}

WebSocketModule::~WebSocketModule()
{

}

bool WebSocketModule::init()
{
	return true;
}

bool WebSocketModule::run()
{
	return true;
}
