#pragma once


#include "interface_header/logic/IAddonsModule.h"
#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/logic/IGameLogicModule.h"	
#include "interface_header/base/IConfigModule.h"
#include <list>


namespace zq{


class ByteBuffer;
class WorldPacket;
class WorldSession;
class AddonsModule : public IAddonsModule
{
	typedef std::list<BannedAddon> BannedAddonList;
	typedef std::list<SavedAddon> SavedAddonsList;
public:
	AddonsModule(ILibManager* p)
	{
		libManager_ = p;
	}

	bool init() override;
	bool initEnd() override;
    bool loadFromDB() override;
	bool saveToDB() override;

public:

	void readAddonsInfo(WorldSession* session, ByteBuffer& data);
	SavedAddon* getAddonInfo(const std::string& name) override;
	void saveAddon(const AddonInfo& info) override;


private:

	SavedAddonsList m_knownAddons;
	BannedAddonList m_bannedAddons;

	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	IGameLogicModule* gameLogicModule_;
	IConfigModule* configModule_;
};

}
