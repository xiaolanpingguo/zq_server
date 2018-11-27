#include "addons_module.h"

#include "server/game/game_cs_server/world_session.h"
#include "baselib/message/config_define.hpp"
#include "dependencies/zlib/zlib.h"

#include "baselib/message/config_define.hpp"
#include "baselib/base_code/format.h"


namespace zq {


bool AddonsModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	gameLogicModule_ = libManager_->findModule<IGameLogicModule> ();
	return true;
}

bool AddonsModule::initEnd()
{
	//IClassPtr logic_class = classModule_->getClass(config::Server::this_name());
	//if (logic_class)
	//{
	//	const auto& objs = logic_class->getAllStaticObjs();
	//	for (const auto& ele : objs)
	//	{
	//		IObjectPtr obj = ele.second;

	//		const auto& name = obj->getString(config::Addons::name());
	//		const auto& crc = obj->getInt(config::Addons::crc());
	//		m_knownAddons.push_back({ name, (uint32)crc });
	//	}
	//}

	return true;
}

bool AddonsModule::loadFromDB()
{
	return true;
}

bool AddonsModule::saveToDB()
{
	return true;
}

void AddonsModule::saveAddon(const AddonInfo& info)
{
	m_knownAddons.push_back({ info.Name, info.CRC });
}

SavedAddon* AddonsModule::getAddonInfo(const std::string& name)
{
	for (auto it = m_knownAddons.begin(); it != m_knownAddons.end(); ++it)
	{
		if (it->Name == name)
			return &(*it);
	}

	return nullptr;
}

void AddonsModule::readAddonsInfo(WorldSession* session, ByteBuffer& data)
{

}



}

