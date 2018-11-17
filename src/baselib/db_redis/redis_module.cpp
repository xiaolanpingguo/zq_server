#include <algorithm>
#include "redis_client.h"
#include "redis_module.h"
#include "baselib/message/config_define.hpp"
#include "config_header/cfg_redis.hpp"
using namespace zq;


RedisModule::RedisModule(ILibManager* p)
{
	libManager_ = p;
}

RedisModule::~RedisModule()
{

}

bool RedisModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	logModule_ = libManager_->findModule<ILogModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	configModule_->create<CSVRedis>("cfg_redis.csv");

	return true;
}

bool RedisModule::initEnd()
{
	const auto& all_row = configModule_->getCsvRowAll<CSVRedis>();
	for (const auto& ele : *all_row)
	{
		if (!addServer(ele.second->name, ele.second->ip, ele.second->port, false))
		{
			ASSERT(false);
			return false;
		}
	}

	/*IClassPtr logic_class = classModule_->getClass(config::Redis::this_name());
	if (logic_class)
	{
		const auto& objs = logic_class->getAllStaticObjs();
		for (const auto& ele : objs)
		{
			IObjectPtr obj = ele.second;

			const int nPort = obj->getInt(config::Redis::port());
			const std::string& strIP = obj->getString(config::Redis::ip());
			const std::string& strAuth = obj->getString(config::Redis::auth());

			if (!addServer(obj->getObjName(), strIP, nPort, false))
			{
				ASSERT(false);
				return false;
			}
		}
	}*/

	return true;
}

bool RedisModule::finalize()
{
	serverMap_.clear();
	return true;
}

bool RedisModule::run()
{
	std::shared_ptr<RedisClient> client = serverMap_.first();
	while (client)
	{
		client->run();

		client = serverMap_.next();
	}
	
	return true;
}

bool RedisModule::addServer(const std::string& id, const std::string& ip, unsigned short port, bool is_syn, const std::string& password)
{
	if (!serverMap_.existElement(id))
	{
		ClientPtr client = std::make_shared<RedisClient>(ip, port);
		if (is_syn)
		{
			client->synConnect();
		}
		else
		{
			client->asynConnect();
		}

		return serverMap_.addElement(id, client);
	}
	else
	{
		return false;
	}
}

RedisClientPtr RedisModule::getClientById(const std::string& strID)
{
	ClientPtr client = serverMap_.getElement(strID);
	if (client && client->enable())
	{
		return std::dynamic_pointer_cast<IRedisClient>(client);
	}

	return nullptr;
}

RedisClientPtr RedisModule::getClientBySuitConsistent()
{
	ClientPtr client = serverMap_.getElementBySuitConsistent();
	if (client && client->enable())
	{
		return std::dynamic_pointer_cast<IRedisClient>(client);
	}

	return nullptr;
}

RedisClientPtr RedisModule::getClientByHash(const std::string& hash)
{
	ClientPtr client = serverMap_.getElementBySuit(hash);
	if (client && client->enable())
	{
		return std::dynamic_pointer_cast<IRedisClient>(client);
	}

	return nullptr;
}









