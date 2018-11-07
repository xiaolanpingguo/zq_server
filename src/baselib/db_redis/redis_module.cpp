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
	elementModule_ = libManager_->findModule<IElementModule>();
	logModule_ = libManager_->findModule<ILogModule>();

	return true;
}

bool RedisModule::initEnd()
{
	std::shared_ptr<IClass> logic_class = classModule_->getElement(config::Redis::this_name());
	if (logic_class)
	{
		const std::vector<std::string>& strIdList = logic_class->getIDList();
		for (size_t i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

			const int nPort = elementModule_->getPropertyInt(strId, config::Redis::port());
			const std::string& strIP = elementModule_->getPropertyString(strId, config::Redis::ip());
			const std::string& strAuth = elementModule_->getPropertyString(strId, config::Redis::auth());

			if (!addServer(strId, strIP, nPort, false))
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









