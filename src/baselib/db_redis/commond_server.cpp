#include "redis_client.h"
using namespace zq;


void RedisClient::FLUSHALL()
{
    RedisCommand cmd(GET_NAME(FLUSHALL));

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply != nullptr)
	{
		
	}
}

void RedisClient::FLUSHDB()
{
    RedisCommand cmd(GET_NAME(FLUSHDB));

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply != nullptr)
	{
		
	}
}

bool RedisClient::AUTH(const std::string& auth)
{
	RedisCommand cmd(GET_NAME(AUTH));
	cmd << auth;

	// if password error, redis will return REDIS_REPLY_ERROR
	// pReply will be null
	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STATUS)
	{
		if (std::string("OK") == std::string(pReply->str, pReply->len) ||
			std::string("ok") == std::string(pReply->str, pReply->len))
		{
			return true;
		}
	}

	return false;
}

bool RedisClient::SELECTDB()
{
	RedisCommand cmd(GET_NAME(SELECTDB));

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply != nullptr)
	{

	}

	return false;
}

bool RedisClient::ROLE(bool& is_master, string_pair_vector& values)
{
	RedisCommand cmd(GET_NAME(ROLE));

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		// 确定服务器是主服务器则搜索从服务器回调连接
		if (strcmp(pReply->element[0]->str, "master") == 0)
		{
			is_master = true;

			for (size_t k = 0; k < pReply->element[2]->elements; k++)
			{
				redisReply* p = pReply->element[2]->element[k];
				values.emplace_back(std::make_pair(std::string(p->element[0]->str), std::string(p->element[1]->str)));
			}
		}
		else
		{
			is_master = false;
		}
	}

	return true;
}

bool RedisClient::INFO(const std::string& param, std::string& outstr)
{
	RedisCommand cmd(GET_NAME(INFO));
	cmd << param;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		outstr = pReply->str;
	}

	return true;
}

