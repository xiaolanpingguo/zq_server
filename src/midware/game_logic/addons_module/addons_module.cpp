#include "addons_module.h"

#include "../server/world_session.h"
#include "dependencies/zlib/zlib.h"
#include "config_header/cfg_addones.hpp"

#include "baselib/base_code/format.h"


namespace zq {


bool AddonsModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	gameLogicModule_ = libManager_->findModule<IGameLogicModule>();
	configModule_ = libManager_->findModule<IConfigModule>();
	configModule_->create<CSVAddOnes>("cfg_addones.csv");
	return true;
}

bool AddonsModule::initEnd()
{
	const auto& all_row = configModule_->getCsvRowAll<CSVAddOnes>();
	for (const auto& ele : *all_row)
	{
		m_knownAddons.emplace_back(SavedAddon{ ele.second.name, ele.second.crc });
	}

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

