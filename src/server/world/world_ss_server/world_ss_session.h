#pragma once 



#include "baselib/network/internal_socket.hpp"
#include "baselib/message/game_ss.pb.h"


namespace SSMsg
{
	class SSPacket;
}

namespace zq {


class WorldSSSession : public InternalSocket
{
public:

	WorldSSSession(tcp::socket&& socket);
	~WorldSSSession();

	void start() override;
	void onClose() override;
	bool readDataHandler() override;

	int getServerId() { return serverInfos_.server_id(); }
	int getServerType() { return serverInfos_.server_type(); }
	const SSMsg::ServerInfo& getServerInfo() { return serverInfos_; }

public:
	bool onS2SServerRegisterReq(const SSMsg::SSPacket& packet);
	bool onS2SHeartBeat(const SSMsg::SSPacket& packet);

private:

	SSMsg::ServerInfo serverInfos_;
};

}
