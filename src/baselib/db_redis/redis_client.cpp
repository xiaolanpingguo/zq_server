#include "redis_client.h"


#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma  comment(lib,"hiredis_d.lib")
#pragma  comment(lib,"Ws2_32.lib")
#endif

using namespace zq;

RedisClient::RedisClient(const std::string& ip, int port)
	:ip_(ip),
	port_(port),
	busy_(false),
	authed_(false),
	lastCheckTime_(0),
	redisConnect_(std::make_shared<RedisClientSocket>(ip_, port_))
{
}

RedisClient::~RedisClient()
{

}

bool RedisClient::enable()
{
	return redisConnect_->isConnect();
}

bool RedisClient::authed()
{
	return authed_;
}

bool RedisClient::busy()
{
	return busy_;
}

void RedisClient::asynConnect()
{
	redisConnect_->asynConnect();
}

void RedisClient::synConnect()
{
	redisConnect_->synConnect();
}

void RedisClient::reConnect()
{
	redisConnect_.reset(new RedisClientSocket(ip_, port_));
	redisConnect_->asynConnect();
}

bool RedisClient::isConnect()
{
	if (redisConnect_)
	{
		return redisConnect_->isConnect();
	}

	return false;
}

bool RedisClient::run()
{
    redisConnect_->run();
	checkConnect();

    return false;
}

void RedisClient::checkConnect()
{
	static constexpr int CHECK_TIME = 10;

	time_t now = time(nullptr);
	if (lastCheckTime_ + CHECK_TIME > now)
	{
		return;
	}

	lastCheckTime_ = now;

	if (!redisConnect_->isConnect())
	{
		reConnect();
	}
}

RedisReplyPtr RedisClient::exeCmd(const RedisCommand& cmd)
{
	while (busy_)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	if (!isConnect())
	{
		reConnect();
		return nullptr;
	}
	else
	{
		std::string msg = cmd.serialize();
		if (msg.empty())
		{
			return nullptr;
		}
		int nRet = redisConnect_->write(msg.data(), msg.length());
		if (nRet != 0)
		{
			return nullptr;
		}
	}
	
	busy_ = true;

	return parseForReply();
}

RedisReplyPtr RedisClient::parseForReply()
{
	struct redisReply* reply = nullptr;
	while (true)
	{
		// 这里必须要有reply != nullptr， 因为如果buf中没有数据，reply会为空
		int ret = redisReaderGetReply(redisConnect_->getRedisReader(), (void**)&reply);
		if (ret == REDIS_OK && reply != nullptr)
		{
			break;
		}

		redisConnect_->run();

		if (!isConnect())
		{
			reConnect();
			break;
		}
	}

	busy_ = false;

	if (reply == nullptr)
	{
		return nullptr;
	}

	if (REDIS_REPLY_ERROR == reply->type)
	{
		freeReplyObject(reply);
		return nullptr;
	}

	return RedisReplyPtr(reply, [](redisReply* r) { if (r) freeReplyObject(r); });
}