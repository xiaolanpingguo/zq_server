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

bool DataAgentModule::getRedisData(const std::string& user_id, const std::string& key, std::string& data)
{
	RedisClientPtr redis_client = redisModule_->getClientByHash(user_id);
	if (redis_client == nullptr)
	{
		return false;
	}

	return redis_client->GET(key, data);
}


}