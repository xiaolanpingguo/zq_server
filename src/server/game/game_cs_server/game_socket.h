#pragma once 


#include "baselib/network/socket.hpp"

namespace zq {


class WorldPacket;
class EncryptablePacket;
class WorldSession;
class GameSocket : public Socket<GameSocket>
{
	typedef Socket<GameSocket> BaseSocket;

public:

	GameSocket(tcp_t::socket&& socket);

	void start() override;
	void onClose() override;
	bool update() override;

	void sendPacket(MessageBuffer&& packet);

protected:

	void readHandler() override;
	bool readHeaderLengthHandler();
	bool readDataHandler();

private:

	WorldSession* _worldSession;

	MessageBuffer headerLengthBuffer_;
	MessageBuffer packetBuffer_;
};

}
