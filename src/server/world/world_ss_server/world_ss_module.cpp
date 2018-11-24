#include "world_ss_module.h"
#include "baselib/message/config_define.hpp"
#include "config_header/cfg_server.hpp"


namespace zq{


WorldSSModule::WorldSSModule(ILibManager* p)
{
	libManager_ = p;
}

bool WorldSSModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	return true;
}

bool WorldSSModule::initEnd()
{
	const auto& all_row = configModule_->getCsvRowAll<CSVServer>();
	for (const auto& ele : *all_row)
	{
		int server_type = ele.second->server_type;
		int server_id = ele.second->server_id;
		if (server_type == SERVER_TYPES::ST_WORLD_SERVER && libManager_->getServerID() == server_id)
		{
			//int max_conn = ele.second->max_conn;
			int int_port = ele.second->internal_port;
			const std::string& int_ip = ele.second->internal_ip;

			if (int_port != -1)
			{
				if (!startNetwork(int_ip, (uint16)int_port))
				{
					ASSERT(false, "Cannot init internal server net");
					return false;
				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------world_ss_server is listen on internal net-----------" << std::endl;
				strLog << "listen ext_address: " << int_ip + ":" + std::to_string(int_port) << std::endl;
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
