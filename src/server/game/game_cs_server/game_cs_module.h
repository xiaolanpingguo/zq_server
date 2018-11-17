#pragma once

#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IGameCSModule.h"
#include "interface_header/base/IConfigModule.h"

#include "baselib/network/socket_mgr.hpp"
#include "game_socket.h"

namespace zq {

class GameCSModule : public IGameCSModule, public SocketMgr<GameSocket>
{
	using BaseSockMgr = SocketMgr<GameSocket>;
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

	IClassModule* classModule_;
	IConfigModule* configModule_;
};

}

