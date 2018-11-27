#include "game_cs_module.h"

#include "config_header/cfg_server.hpp"


namespace zq {


bool GameCSModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	return true;
}

bool GameCSModule::initEnd()
{	
	const auto& all_row = configModule_->getCsvRowAll<CSVServer>();
	for (const auto& ele : *all_row)
	{
		int server_type = ele.second.server_type;
		int server_id = ele.second.server_id;
		if (server_type == SERVER_TYPES::ST_GAME_SERVER && libManager_->getServerID() == server_id)
		{
			//int max_conn = ele.second->max_conn;
			int ext_port = ele.second.external_port;
			const std::string& ext_ip = ele.second.external_ip;

			if (ext_port != -1)
			{
				if (!startNetwork(ext_ip, (uint16)ext_port))
				{
					ASSERT(false, "Cannot init external server net");
					return false;
				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------game_cs_server is listen on external net-----------" << std::endl;
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

