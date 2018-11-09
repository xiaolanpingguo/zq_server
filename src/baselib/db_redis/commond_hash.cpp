#include "redis_client.h"
using namespace zq;


int RedisClient::HDEL(const std::string& key, const std::string &field)
{
    RedisCommand cmd(GET_NAME(HDEL));
    cmd << key;
    cmd << field;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int del_num = 0; 
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		del_num = (int)pReply->integer;
	}

	return del_num;
}

int RedisClient::HDEL(const std::string &key, const string_vector& fields)
{
	RedisCommand cmd(GET_NAME(HDEL));
	cmd << key;
	for (auto it = fields.begin(); it != fields.end(); ++it)
	{
		cmd << *it;
	}

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int del_num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		del_num = (int)pReply->integer;
	}

	return del_num;
}

bool RedisClient::HEXISTS(const std::string &key, const std::string &field)
{
    RedisCommand cmd(GET_NAME(HEXISTS));
    cmd << key;
    cmd << field;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool exist = false;
	if (pReply->type == REDIS_REPLY_INTEGER && pReply->integer == 1)
	{
		exist = true;
	}

	return exist;
}

bool RedisClient::HGET(const std::string& key, const std::string& field, std::string& value)
{
	RedisCommand cmd(GET_NAME(HGET));
	cmd << key;
	cmd << field;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(pReply->str, pReply->len);
		return true;
	}
	else
	{
		return false;
	}
}

bool RedisClient::HGETALL(const std::string &key, std::vector<string_pair> &values)
{
    RedisCommand cmd(GET_NAME(HGETALL));
    cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (int k = 0; k < (int)pReply->elements; k = k + 2)
		{
			values.emplace_back(std::move(string_pair{ std::string(pReply->element[k]->str, pReply->element[k]->len),
				std::string(pReply->element[k + 1]->str, pReply->element[k + 1]->len) }));
		}

		return true;
	}

	return false;
}

bool RedisClient::HINCRBY(const std::string &key, const std::string &field, const int by, int64_t& value)
{
    RedisCommand cmd(GET_NAME(HINCRBY));
    cmd << key;
    cmd << field;
    cmd << by;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		value = pReply->integer;

		return true;
	}

	return false;
}

bool RedisClient::HINCRBYFLOAT(const std::string &key, const std::string &field, const float by, float& value)
{
    RedisCommand cmd(GET_NAME(HINCRBYFLOAT));
    cmd << key;
    cmd << field;
    cmd << by;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (pReply->type == REDIS_REPLY_STRING)
	{
		success = strto<float>(pReply->str, value);
	}

	return success;
}

bool RedisClient::HKEYS(const std::string &key, std::vector<std::string> &fields)
{
    RedisCommand cmd(GET_NAME(HKEYS));
    cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (int k = 0; k < (int)pReply->elements; k++)
		{
			fields.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
		}
		
		return true;
	}

	return false;
}

bool RedisClient::HLEN(const std::string &key, int& number)
{
    RedisCommand cmd(GET_NAME(HLEN));
    cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		number = (int)pReply->integer;
		return true;
	}

	return false;
}

bool RedisClient::HMGET(const std::string &key, const string_vector &fields, string_vector &values)
{
    RedisCommand cmd(GET_NAME(HMGET));
    cmd << key;
    for (size_t i = 0; i < fields.size(); ++i)
    {
        cmd << fields[i];
    }

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (int k = 0; k < (int)pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				values.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}

		return true;
	}

	return false;
}

bool RedisClient::HMSET(const std::string &key, const std::vector<string_pair> &values)
{
    RedisCommand cmd(GET_NAME(HMSET));
    cmd << key;
    for (size_t i = 0; i < values.size(); ++i)
    {
        cmd << values[i].first;
        cmd << values[i].second;
    }

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::HMSET(const std::string& key, const string_vector& fields, const string_vector& values)
{
	if (fields.size() != values.size())
	{
		return false;
	}

	RedisCommand cmd(GET_NAME(HMSET));
	cmd << key;
	for (size_t i = 0; i < values.size(); ++i)
	{
		cmd << fields[i];
		cmd << values[i];
	}

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::HSET(const std::string &key, const std::string &field, const std::string &value)
{
    RedisCommand cmd(GET_NAME(HSET));
    cmd << key;
    cmd << field;
    cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		success = (bool)pReply->integer;
	}

	return success;
}

bool RedisClient::HSETNX(const std::string &key, const std::string &field, const std::string &value)
{
    RedisCommand cmd(GET_NAME(HSETNX));
    cmd << key;
    cmd << field;
    cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		success = (bool)pReply->integer;
	}

	return success;
}

bool RedisClient::HVALS(const std::string &key, string_vector &values)
{
    RedisCommand cmd(GET_NAME(HVALS));
    cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (int k = 0; k < (int)pReply->elements; k++)
		{
			values.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
		}
	}

	return true;
}

bool RedisClient::HSTRLEN(const std::string &key, const std::string &field, int& length)
{
    RedisCommand cmd(GET_NAME(HSTRLEN));
    cmd << key;
    cmd << field;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		length = (int)pReply->integer;
		return true;
	}

	return false;
}
