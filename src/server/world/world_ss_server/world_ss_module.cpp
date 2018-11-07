#include "world_ss_module.h"
#include "baselib/message/config_define.hpp"


namespace zq{


WorldSSModule::WorldSSModule(ILibManager* p)
{
	libManager_ = p;
}

bool WorldSSModule::init()
{
	elementModule_ = libManager_->findModule<IElementModule>();
	classModule_ = libManager_->findModule<IClassModule>();

	return true;
}

bool WorldSSModule::initEnd()
{
	std::shared_ptr<IClass> logic_class = classModule_->getElement(config::Server::this_name());
	if (logic_class)
	{
		const std::vector<std::string>& strIdList = logic_class->getIDList();
		for (size_t i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

			int server_type = elementModule_->getPropertyInt(strId, config::Server::server_type());
			int server_id = elementModule_->getPropertyInt(strId, config::Server::server_id());
			if (server_type == SERVER_TYPES::ST_WORLD_SERVER && libManager_->getServerID() == server_id)
			{
				int int_port = elementModule_->getPropertyInt(strId, config::Server::internal_port());
				const std::string& int_ip = elementModule_->getPropertyString(strId, config::Server::internal_ip());

				if (int_port != -1)
				{
					if (!startNetwork(int_ip, (uint16)int_port))
					{
						ASSERT(false, "Cannot init internal server net");
						return false;
					}
				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------wolrd_server is listen on internal net-----------" << std::endl;
				strLog << "listen int_address: " << int_ip + ":" + std::to_string(int_port) << std::endl;
				std::cout << strLog.str() << std::endl;
				LOG_INFO << strLog.str();
			}
		}
	}

	return true;
}

bool WorldSSModule::run()
{
	update();
	return true;
}

WorldSSSessionPtr WorldSSModule::getSessionByServerId(int server_id)
{
	for (const auto& ref : socketMap_)
	{
		if (ref.second->getServerId() == server_id)
		{
			return ref.second;
		}
	}

	return nullptr;
}

void WorldSSModule::getSessionByServerType(int server_type, WorldSSSessionList& vec)
{
	for (const auto& ref : socketMap_)
	{
		if (ref.second->getServerType() == server_type)
		{
			vec.emplace_back(ref.second);
		}
	}
}

}
