#include "game_cs_module.h"

#include "baselib/message/config_define.hpp"


namespace zq {


bool GameCSModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();

	return true;
}

bool GameCSModule::initEnd()
{	
	IClassPtr logic_class = classModule_->getClass(config::Server::this_name());
	if (logic_class)
	{
		const auto& objs = logic_class->getAllObjs();
		for (const auto& obj : objs)
		{
			auto property_mgr = obj.second;

			int server_type = property_mgr->getPropertyInt(config::Server::server_type());
			int server_id = property_mgr->getPropertyInt(config::Server::server_id());
			if (server_type == SERVER_TYPES::ST_GAME_SERVER && libManager_->getServerID() == server_id)
			{
				//int max_conn = property_mgr->getPropertyInt(config::Server::max_connect());
				int ext_port = property_mgr->getPropertyInt(config::Server::external_port());
				const std::string& ext_ip = property_mgr->getPropertyString(config::Server::external_ip());

				if (ext_port != -1)
				{
					if (!startNetwork(ext_ip, (uint16)ext_port))
					{
						ASSERT(false, "Cannot init external server net");
						return false;
					}

				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------login_server is listen on external net-----------" << std::endl;
				strLog << "listen ext_address: " << ext_ip + ":" + std::to_string(ext_port) << std::endl;
				std::cout << strLog.str() << std::endl;
				LOG_INFO << strLog.str();
			}
		}
	}

	return true;
}

bool GameCSModule::run()
{
	update();
	return true;
}

}

