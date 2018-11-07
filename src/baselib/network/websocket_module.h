#pragma once


#include "baselib/interface_header/IWebsocketModule.h"


namespace zq {

class WebSocketModule : public IWebSocketModule
{

public:
	WebSocketModule(ILibManager* p);

	virtual ~WebSocketModule();

public:

	bool init();

	bool run() override;
};

}
