#pragma once 

#include "baselib/network/client_socket.hpp"

namespace SSMsg
{
	class SSPacket;
}

namespace zq {


class GameToWorldSession : public ClientSocket
{
public:

	GameToWorldSession(const std::string& ip, uint16 port, int serverId);

	void start() override;
	void onClose() override;

	int getServerId() { return serverId_; }

public:

	void registerToComponent();
	bool onS2SHeartBeat(const SSMsg::SSPacket& packet);

private:
	bool readDataHandler() override;

private:

	int serverId_;
};

}
