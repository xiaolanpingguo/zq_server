#include "data_agent_module.h"

namespace zq {


bool DataAgentModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	redisModule_ = libManager_->findModule<IRedisModule>();

	return true;
}

bool DataAgentModule::run()
{

	return true;
}

bool DataAgentModule::setRedisHashData(const std::string& key, const std::string& field_key, const std::string& data)
{
	RedisClientPtr redis_client = redisModule_->getClientByHash(key);
	if (redis_client == nullptr)
	{
		return false;
	}

	return redis_client->HSET(key, field_key, data);
}

bool DataAgentModule::getRedisHashData(const std::string& key, const std::string& field_key, std::string& data)
{
	RedisClientPtr redis_client = redisModule_->getClientByHash(key);
	if (redis_client == nullptr)
	{
		return false;
	}

	return redis_client->HGET(key, field_key, data);
}

bool DataAgentModule::hexists(const std::string& user_id, const std::string& field_key)
{
	std::string key = _KEY_USER_ + user_id;
	RedisClientPtr redis_client = redisModule_->getClientByHash(key);
	if (redis_client == nullptr)
	{
		return false;
	}

	return redis_client->HEXISTS(key, field_key);
}


}