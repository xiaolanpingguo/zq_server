#include "redis_client.h"
using namespace zq;


bool RedisClient::PUBLISH(const std::string& key, const std::string& value)
{
	RedisCommand cmd(GET_NAME(PUBLISH));
	cmd << key;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true; 
}

bool RedisClient::SUBSCRIBE(const std::string& key)
{
	RedisCommand cmd(GET_NAME(SUBSCRIBE));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::UNSUBSCRIBE(const std::string& key)
{
	RedisCommand cmd(GET_NAME(UNSUBSCRIBE));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}
