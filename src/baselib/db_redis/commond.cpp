#include "redis_client.h"

namespace zq{

#pragma region string
//----------------------------string--------------------------------------
bool RedisClient::APPEND(const std::string &key, const std::string &value, int& length)
{
	RedisCommand cmd(GET_NAME(APPEND));
    cmd << key;
    cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		length = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::DECR(const std::string& key, int64_t& value)
{
	RedisCommand cmd(GET_NAME(DECR));
    cmd << key;

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

bool RedisClient::DECRBY(const std::string &key, const int64_t decrement, int64_t& value)
{
    RedisCommand cmd(GET_NAME(DECRBY));
    cmd << key;
    cmd << decrement;

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

bool RedisClient::GETSET(const std::string &key, const std::string &value, std::string &oldValue)
{
    RedisCommand cmd(GET_NAME(GETSET));
    cmd << key;
    cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		oldValue = pReply->str;
		
		return true;
	}

	return false;
}

bool RedisClient::INCR(const std::string &key, int64_t& value)
{
    RedisCommand cmd(GET_NAME(INCR));
    cmd << key;

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

bool RedisClient::INCRBY(const std::string &key, const int64_t increment, int64_t& value)
{
    RedisCommand cmd(GET_NAME(INCRBY));
    cmd << key;
    cmd << increment;

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

bool RedisClient::INCRBYFLOAT(const std::string &key, const float increment, float& value)
{
    RedisCommand cmd(GET_NAME(INCRBYFLOAT));
    cmd << key;
    cmd << increment;

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

bool RedisClient::MGET(const string_vector& keys, string_vector& values)
{
	RedisCommand cmd(GET_NAME(MGET));

    for (size_t i = 0; i < keys.size(); ++i)
    {
        cmd << keys[i];
    }

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				values.emplace_back(std::move(std::string(pReply->element[k]->str)));
			}
		}
	
		return true;
	}

	return false;
}

void RedisClient::MSET(const string_pair_vector &values)
{
    RedisCommand cmd(GET_NAME(MSET));

    for (size_t i = 0; i < values.size(); ++i)
    {
        cmd << values[i].first;
        cmd << values[i].second;
    }

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply != nullptr)
	{
		
	}
}

bool RedisClient::SETEX(const std::string &key, const std::string &value, int time)
{
    RedisCommand cmd(GET_NAME(SETEX));
    cmd << key;
    cmd << value;
    cmd << time;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::SETNX(const std::string &key, const std::string &value)
{
    RedisCommand cmd(GET_NAME(SETNX));
    cmd << key;
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

bool RedisClient::STRLEN(const std::string &key, int& length)
{
    RedisCommand cmd(GET_NAME(STRLEN));
    cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		length = (int)pReply->integer;
		
		return length != 0;
	}

	return false;
}

bool RedisClient::SET(const std::string &key, const std::string &value)
{
    RedisCommand cmd(GET_NAME(SET));
    cmd << key;
    cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::GET(const std::string& key, std::string& value)
{
    RedisCommand cmd(GET_NAME(GET));
    cmd << key;

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

	return false;
}
//----------------------------string  end--------------------------------------
#pragma endregion string




#pragma region hash
//----------------------------hash-------------------------------------
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

//----------------------------hash end-------------------------------------
#pragma endregion hash



#pragma region key
//----------------------------key--------------------------------------
bool RedisClient::DEL(const std::string &key)
{
	RedisCommand cmd(GET_NAME(DEL));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool del_key_num = false;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		del_key_num = (bool)pReply->integer;
	}

	return del_key_num;
}

bool RedisClient::EXISTS(const std::string &key)
{
	RedisCommand cmd(GET_NAME(EXISTS));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool exist = false;
	if (REDIS_REPLY_INTEGER == pReply->type && 1 == pReply->integer)
	{
		exist = true;
	}

	return exist;
}

bool RedisClient::EXPIRE(const std::string &key, const unsigned int secs)
{
	RedisCommand cmd(GET_NAME(EXPIRE));
	cmd << key;
	cmd << secs;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (REDIS_REPLY_INTEGER == pReply->type && 1 == pReply->integer)
	{
		success = true;
	}

	return success;
}

bool RedisClient::EXPIREAT(const std::string &key, const int64_t unixTime)
{
	RedisCommand cmd(GET_NAME(EXPIREAT));
	cmd << key;
	cmd << unixTime;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (REDIS_REPLY_INTEGER == pReply->type && 1 == pReply->integer)
	{
		success = true;
	}

	return success;
}

bool RedisClient::PERSIST(const std::string &key)
{
	RedisCommand cmd(GET_NAME(PERSIST));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (REDIS_REPLY_INTEGER == pReply->type && 1 == pReply->integer)
	{
		success = true;
	}

	return success;
}

int RedisClient::TTL(const std::string &key)
{
	RedisCommand cmd(GET_NAME(TTL));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int left_time = -1;
	if (REDIS_REPLY_INTEGER == pReply->type)
	{
		left_time = (int)pReply->integer;
	}

	return left_time;
}

std::string RedisClient::TYPE(const std::string &key)
{
	RedisCommand cmd(GET_NAME(TYPE));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return "";
	}

	std::string type_name = "";
	if (pReply->type == REDIS_REPLY_STATUS)
	{
		type_name = pReply->str;
	}

	return type_name;
}

//----------------------------key end--------------------------------------
#pragma endregion key



#pragma region list
//----------------------------list--------------------------------------
bool RedisClient::LINDEX(const std::string &key, const int index, std::string& value)
{
	RedisCommand cmd(GET_NAME(LINDEX));
	cmd << key;
	cmd << index;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(pReply->str, pReply->len);
	}

	return true;
}

bool RedisClient::LLEN(const std::string &key, int& length)
{
	RedisCommand cmd(GET_NAME(LLEN));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		length = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::LPOP(const std::string &key, std::string& value)
{
	RedisCommand cmd(GET_NAME(LPOP));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(pReply->str, pReply->len);
	}

	return true;
}

int RedisClient::LPUSH(const std::string &key, const std::string &value)
{
	RedisCommand cmd(GET_NAME(LPUSH));
	cmd << key;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int list_len = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		list_len = (int)pReply->integer;
	}

	return list_len;
}

int RedisClient::LPUSHX(const std::string &key, const std::string &value)
{
	RedisCommand cmd(GET_NAME(LPUSHX));
	cmd << key;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int list_len = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		list_len = (int)pReply->integer;
	}

	return list_len;
}

bool RedisClient::LRANGE(const std::string &key, const int start, const int end, string_vector &values)
{
	if (end - start <= 0)
	{
		return false;
	}

	RedisCommand cmd(GET_NAME(LRANGE));
	cmd << key;
	cmd << start;
	cmd << end;

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
	}

	return true;
}

bool RedisClient::LSET(const std::string &key, const int index, const std::string &value)
{
	RedisCommand cmd(GET_NAME(LSET));
	cmd << key;
	cmd << index;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	return true;
}

bool RedisClient::RPOP(const std::string &key, std::string& value)
{

	RedisCommand cmd(GET_NAME(RPOP));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(pReply->str, pReply->len);
	}

	return true;
}

int RedisClient::RPUSH(const std::string &key, const std::string &value)
{
	RedisCommand cmd(GET_NAME(RPUSH));
	cmd << key;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int list_len = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		list_len = (int)pReply->integer;
	}

	return list_len;
}

int RedisClient::RPUSHX(const std::string &key, const std::string &value)
{

	RedisCommand cmd(GET_NAME(RPUSHX));
	cmd << key;
	cmd << value;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int list_len = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		list_len = (int)pReply->integer;
	}

	return list_len;
}

//----------------------------list end--------------------------------------
#pragma endregion list



#pragma region pubsub
//----------------------------pubsub--------------------------------------
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

//----------------------------pubsub end--------------------------------------
#pragma endregion pubsub



#pragma region server
//----------------------------server--------------------------------------
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
//----------------------------end--------------------------------------
#pragma endregion server



#pragma region set
//----------------------------set--------------------------------------
int RedisClient::SADD(const std::string& key, const std::string& member)
{
	RedisCommand cmd(GET_NAME(SADD));
	cmd << key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int add_new_num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		add_new_num = (int)pReply->integer;
	}

	return add_new_num;
}

bool RedisClient::SCARD(const std::string& key, int& nCount)
{
	RedisCommand cmd(GET_NAME(SCARD));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		nCount = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::SDIFF(const std::string& key_1, const std::string& key_2, string_vector& output)
{
	RedisCommand cmd(GET_NAME(SDIFF));
	cmd << key_1;
	cmd << key_2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}
	}

	return true;
}

int RedisClient::SDIFFSTORE(const std::string& store_key, const std::string& diff_key1, const std::string& diff_key2)
{
	RedisCommand cmd(GET_NAME(SDIFFSTORE));
	cmd << store_key;
	cmd << diff_key1;
	cmd << diff_key2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		num = (int)pReply->integer;
	}

	return num;
}

bool RedisClient::SINTER(const std::string& key_1, const std::string& key_2, string_vector& output)
{
	RedisCommand cmd(GET_NAME(SINTER));
	cmd << key_1;
	cmd << key_2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}
	}

	return true;
}

int RedisClient::SINTERSTORE(const std::string& inter_store_key, const std::string& inter_key1, const std::string& inter_key2)
{
	RedisCommand cmd(GET_NAME(SINTERSTORE));
	cmd << inter_store_key;
	cmd << inter_key1;
	cmd << inter_key2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		num = (int)pReply->integer;
	}

	return num;
}

bool RedisClient::SISMEMBER(const std::string& key, const std::string& member)
{
	RedisCommand cmd(GET_NAME(SISMEMBER));
	cmd << key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER && pReply->integer == 1)
	{
		return true;
	}

	return false;
}

bool RedisClient::SMEMBERS(const std::string& key, string_vector& output)
{
	RedisCommand cmd(GET_NAME(SMEMBERS));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}
	}

	return true;
}

bool RedisClient::SMOVE(const std::string& source_key, const std::string& dest_key, const std::string& member)
{
	RedisCommand cmd(GET_NAME(SMOVE));
	cmd << source_key;
	cmd << dest_key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		return (bool)pReply->integer;
	}

	return false;
}

bool RedisClient::SPOP(const std::string& key, std::string& output)
{
	RedisCommand cmd(GET_NAME(SPOP));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STRING)
	{
		output = std::string(pReply->str, pReply->len);
		return true;
	}

	return false;
}

bool RedisClient::SRANDMEMBER(const std::string& key, int count, string_vector& output)
{
	RedisCommand cmd(GET_NAME(SRANDMEMBER));
	cmd << key;
	cmd << count;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}
	}

	return true;
}

int RedisClient::SREM(const std::string& key, const string_vector& members)
{
	RedisCommand cmd(GET_NAME(SREM));
	cmd << key;
	for (const auto& member : members)
	{
		cmd << member;
	}

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		num = (int)pReply->integer;
	}

	return num;
}

bool RedisClient::SUNION(const std::string& union_key1, const std::string& union_key2, string_vector& output)
{
	RedisCommand cmd(GET_NAME(SUNION));
	cmd << union_key1;
	cmd << union_key2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t k = 0; k < pReply->elements; k++)
		{
			if (pReply->element[k]->type == REDIS_REPLY_STRING)
			{
				output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
			}
		}
	}

	return true;
}

int RedisClient::SUNIONSTORE(const std::string& dest_store_key, const std::string& union_key1, const std::string& union_key2)
{
	RedisCommand cmd(GET_NAME(SUNIONSTORE));
	cmd << dest_store_key;
	cmd << union_key1;
	cmd << union_key2;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		num = (int)pReply->integer;
	}

	return num;
}
//----------------------------set end--------------------------------------
#pragma endregion set



#pragma region sort
//----------------------------sort--------------------------------------
int RedisClient::ZADD(const std::string& key, const std::string& member, const double score)
{
	RedisCommand cmd(GET_NAME(ZADD));
	cmd << key;
	cmd << score;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	int add_new_num = 0;
	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		add_new_num = (int)pReply->integer;
	}

	return add_new_num;
}

bool RedisClient::ZCARD(const std::string& key, int &nCount)
{
	RedisCommand cmd(GET_NAME(ZCARD));
	cmd << key;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		nCount = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::ZCOUNT(const std::string& key, const double start, const double end, int& nCount)
{
	RedisCommand cmd(GET_NAME(ZCOUNT));
	cmd << key;
	cmd << start;
	cmd << end;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		nCount = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::ZINCRBY(const std::string& key, const std::string& member, const double score, double& newScore)
{
	RedisCommand cmd(GET_NAME(ZINCRBY));
	cmd << key;
	cmd << score;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (pReply->type == REDIS_REPLY_STRING)
	{
		success = strto<double>(pReply->str, newScore);
	}

	return success;
}

bool RedisClient::ZRANGE(const std::string& key, const int start, const int end, string_score_vector& values)
{
	RedisCommand cmd(GET_NAME(ZRANGE));
	cmd << key;
	cmd << start;
	cmd << end;
	cmd << "WITHSCORES";

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	try
	{
		if (pReply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t k = 0; k < pReply->elements; k = k + 2)
			{
				if (pReply->element[k]->type == REDIS_REPLY_STRING)
				{
					string_score_pair vecPair;
					vecPair.first = std::string(pReply->element[k]->str, pReply->element[k]->len);
					vecPair.second = lexical_cast<double>(pReply->element[k + 1]->str);
					values.emplace_back(vecPair);
				}
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool RedisClient::ZRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values)
{
	RedisCommand cmd(GET_NAME(ZRANGEBYSCORE));
	cmd << key;
	cmd << key;
	cmd << start;
	cmd << end;
	cmd << "WITHSCORES";

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	try
	{
		if (pReply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t k = 0; k < pReply->elements; k = k + 2)
			{
				if (pReply->element[k]->type == REDIS_REPLY_STRING)
				{
					string_score_pair vecPair;
					vecPair.first = std::string(pReply->element[k]->str, pReply->element[k]->len);
					vecPair.second = lexical_cast<double>(pReply->element[k + 1]->str);
					values.emplace_back(vecPair);
				}
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool RedisClient::ZRANK(const std::string & key, const std::string & member, int& rank)
{
	RedisCommand cmd(GET_NAME(ZRANK));
	cmd << key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		rank = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::ZREM(const std::string & key, const std::string & member)
{
	RedisCommand cmd(GET_NAME(ZREM));
	cmd << key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		return (bool)pReply->integer;
	}

	return false;
}

bool RedisClient::ZREMRANGEBYRANK(const std::string & key, const int start, const int end)
{
	RedisCommand cmd(GET_NAME(ZREMRANGEBYRANK));
	cmd << key;
	cmd << start;
	cmd << end;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		return (bool)pReply->integer;
	}

	return false;
}

bool RedisClient::ZREMRANGEBYSCORE(const std::string & key, const double start, const double end)
{
	RedisCommand cmd(GET_NAME(ZREMRANGEBYSCORE));
	cmd << key;
	cmd << start;
	cmd << end;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		return (bool)pReply->integer;
	}

	return false;
}

bool RedisClient::ZREVRANGE(const std::string& key, const int start, const int end, string_score_vector& values)
{
	RedisCommand cmd(GET_NAME(ZREVRANGE));
	cmd << key;
	cmd << start;
	cmd << end;
	cmd << "WITHSCORES";

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	try
	{
		if (pReply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t k = 0; k < pReply->elements; k = k + 2)
			{
				if (pReply->element[k]->type == REDIS_REPLY_STRING)
				{
					string_score_pair vecPair;
					vecPair.first = std::string(pReply->element[k]->str, pReply->element[k]->len);
					vecPair.second = lexical_cast<double>(pReply->element[k + 1]->str);
					values.emplace_back(vecPair);
				}
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool RedisClient::ZREVRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values)
{
	RedisCommand cmd(GET_NAME(ZREVRANGEBYSCORE));
	cmd << key;
	cmd << start;
	cmd << end;
	cmd << "WITHSCORES";

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	try
	{
		if (pReply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t k = 0; k < pReply->elements; k = k + 2)
			{
				if (pReply->element[k]->type == REDIS_REPLY_STRING)
				{
					string_score_pair vecPair;
					vecPair.first = std::string(pReply->element[k]->str, pReply->element[k]->len);
					vecPair.second = lexical_cast<double>(pReply->element[k + 1]->str);
					values.emplace_back(vecPair);
				}
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool RedisClient::ZREVRANK(const std::string & key, const std::string & member, int& rank)
{
	RedisCommand cmd(GET_NAME(ZREVRANK));
	cmd << key;
	cmd << member;

	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		rank = (int)pReply->integer;
	}

	return true;
}

bool RedisClient::ZSCORE(const std::string & key, const std::string & member, double& score)
{
	RedisCommand cmd(GET_NAME(ZSCORE));
	cmd << key;
	cmd << member;


	RedisReplyPtr pReply = exeCmd(cmd);
	if (pReply == nullptr)
	{
		return false;
	}

	bool success = false;
	if (pReply->type == REDIS_REPLY_STRING)
	{
		success = strto<double>(pReply->str, score);
	}

	return success;
}
//----------------------------sort end--------------------------------------
#pragma endregion sort

}
