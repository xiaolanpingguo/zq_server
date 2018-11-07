#include "redis_client.h"
#include "redis_socket.h"
using namespace zq;

//1048576 = 1024 * 1024
#define REDIS_BUFFER_MAX_READ (1024 * 1024)

static Asio::IoContext s_ioContext{ 1 };
RedisClientSocket::RedisClientSocket(const std::string& ip, uint16 port)
	: ClientSocket(s_ioContext, ip, port)
{
	redisReader_ = redisReaderCreate();
}

RedisClientSocket::~RedisClientSocket()
{
	if (redisReader_)
	{
		redisReaderFree(redisReader_);
		redisReader_ = nullptr;
	}
}

int RedisClientSocket::run()
{
	ClientSocket::update();
	return 0;
}

bool RedisClientSocket::isConnect()
{
	return isOpen();
}

redisReader* RedisClientSocket::getRedisReader()
{
	return redisReader_;
}

int RedisClientSocket::write(const char* buf, size_t count)
{
	if (buf == nullptr || count <= 0)
	{
		return 0;
	}

	if (!isOpen())
	{
		return -1;
	}

	ClientSocket::BaseSocket::sendData(buf, count);

	return 0;
}

void RedisClientSocket::readHandler()
{
	MessageBuffer& buffer = getReadBuffer();
	size_t len = buffer.getActiveSize();
	if (len > 0)
	{
		redisReaderFeed(redisReader_, (const char*)buffer.getReadPointer(), len);
		buffer.readCompleted(len);
	}

	asyncRead();
}