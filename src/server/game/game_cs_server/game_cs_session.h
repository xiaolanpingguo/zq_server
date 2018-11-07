#pragma once 


#include "baselib/network/socket.hpp"


namespace zq {

class GameCSSession : public Socket<GameCSSession>
{
	typedef Socket<GameCSSession> BaseSocket;

public:

	GameCSSession(tcp::socket&& socket);

	void start() override;
	bool update() override;

protected:
	void readHandler() override;

private:

};

}
