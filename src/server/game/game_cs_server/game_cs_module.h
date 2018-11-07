#pragma once

#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/IGameCSModule.h"

#include "baselib/network/socket_mgr.hpp"
#include "game_cs_session.h"

namespace zq {

class GameCSModule : public IGameCSModule, public SocketMgr<GameCSSession>
{
	using BaseSockMgr = SocketMgr<GameCSSession>;
public:
	GameCSModule(ILibManager* p)
	{
		libManager_ = p;
	}

	bool init() override;
	bool initEnd() override;
	bool run() override;

	const std::string& getIp() override { return BaseSockMgr::getIp(); };
	uint16 getPort() override { return BaseSockMgr::getPort(); };

private:

	IElementModule * elementModule_;
	IClassModule* classModule_;
};

}

