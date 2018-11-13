#include "world_ss_module.h"
#include "baselib/message/config_define.hpp"


namespace zq{


WorldSSModule::WorldSSModule(ILibManager* p)
{
	libManager_ = p;
}

bool WorldSSModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();

	return true;
}

bool WorldSSModule::initEnd()
{

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
