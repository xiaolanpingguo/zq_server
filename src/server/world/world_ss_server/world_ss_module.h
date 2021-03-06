#pragma once


#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IWorldSSModule.h"
#include "interface_header/base/IConfigModule.h"

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

	IClassModule* classModule_;
	IConfigModule* configModule_;
};

}

