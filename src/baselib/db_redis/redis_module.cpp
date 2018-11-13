#include <algorithm>
#include "redis_client.h"
#include "redis_module.h"
#include "baselib/message/config_define.hpp"
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

	return true;
}

bool RedisModule::initEnd()
{
	IClassPtr logic_class = classModule_->getClass(config::Redis::this_name());
	if (logic_class)
	{
		const auto& objs = logic_class->getAllObjs();
		for (const auto& obj : objs)
		{
			auto property_mgr = obj.second;

			const int nPort = property_mgr->getPropertyInt(config::Redis::port());
			const std::string& strIP = property_mgr->getPropertyString(config::Redis::ip());
			const std::string& strAuth = property_mgr->getPropertyString(config::Redis::auth());

			if (!addServer(obj.first, strIP, nPort, false))
			{
				ASSERT(false);
				return false;
			}
		}
	}

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









