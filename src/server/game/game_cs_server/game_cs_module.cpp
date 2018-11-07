#include "game_cs_module.h"

#include "baselib/message/config_define.hpp"


namespace zq {


bool GameCSModule::init()
{
	elementModule_ = libManager_->findModule<IElementModule>();
	classModule_ = libManager_->findModule<IClassModule>();

	return true;
}

bool GameCSModule::initEnd()
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
			if (server_type == SERVER_TYPES::ST_LOGIN_SERVER && libManager_->getServerID() == server_id)
			{
				//int max_conn = elementModule_->getPropertyInt(strId, config::Server::max_connect());
				int ext_port = elementModule_->getPropertyInt(strId, config::Server::external_port());
				const std::string& ext_ip = elementModule_->getPropertyString(strId, config::Server::external_ip());

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

	return true;
}

}

