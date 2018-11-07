#pragma once

#include <memory>
#include <time.h>
#include "lib/interface_header/platform.h"
#include "lib/network/client_socket.hpp"

#include <hiredis/hiredis.h>

namespace zq {


class RedisClientSocket	: public ClientSocket
{
public:
	RedisClientSocket(const std::string& ip, uint16 port);
	virtual ~RedisClientSocket();

	int run();

	int write(const char *buf, size_t count);

	bool isConnect();

	redisReader* getRedisReader();

protected:

	void readHandler() override;

private:

	tcp::endpoint ep_;
	redisReader* redisReader_;
};

}


