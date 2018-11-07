#pragma once


#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/IWorldSSModule.h"

#include "baselib/network/socket_mgr.hpp"

#include "world_ss_session.h"


namespace zq {


class WorldSSModule : public IWorldSSModule, public SocketMgr<WorldSSSession>
{

public:
	WorldSSModule(ILibManager* p);

	bool init() override;
	bool initEnd() override;
	bool run() override;

protected:

	WorldSSSessionPtr getSessionByServerId(int server_id) override;
	void getSessionByServerType(int server_type, WorldSSSessionList& vec) override;

private:

	IElementModule * elementModule_;
	IClassModule* classModule_;
};

}

