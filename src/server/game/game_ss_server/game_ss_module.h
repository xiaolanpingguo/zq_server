#pragma once


#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/IGameSSModule.h"

#include "baselib/network/socket_mgr.hpp"

#include "game_ss_session.h"


namespace zq {


class GameSSModule : public IGameSSModule, public SocketMgr<GameSSSession>
{

public:
	GameSSModule(ILibManager* p);

	bool init() override;
	bool initEnd() override;
	bool run() override;

protected:

	GameSSSessionPtr getSessionByServerId(int server_id) override;
	void getSessionByServerType(int server_type, GameSSSessionList& vec) override;

private:

	IElementModule * elementModule_;
	IClassModule* classModule_;
};

}

