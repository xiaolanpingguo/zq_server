#pragma once


#include "../base/IModule.h"
#include <vector>

namespace zq {


#define STANDARD_ADDON_CRC 0x4C1C776D

struct AddonInfo
{
	AddonInfo(const std::string& name, uint8 enabled, uint32 crc, uint8 state, bool crcOrPubKey)
		: Name(name), Enabled(enabled), CRC(crc), State(state), UsePublicKeyOrCRC(crcOrPubKey)
	{ }

	std::string Name;
	uint8 Enabled;
	uint32 CRC;
	uint8 State;
	bool UsePublicKeyOrCRC;
};

struct SavedAddon
{
	std::string Name;
	uint32 CRC;
};

struct BannedAddon
{
	uint32 Id;
	uint8 NameMD5[16];
	uint8 VersionMD5[16];
	uint32 Timestamp;
};


using AddonsList = std::vector<AddonInfo>;
class ByteBuffer;
class WorldSession;
class IAddonsModule : public IModule
{
public:

	virtual void readAddonsInfo(WorldSession* session, ByteBuffer& data) = 0;
	virtual SavedAddon* getAddonInfo(const std::string& name) = 0;
	virtual void saveAddon(const AddonInfo& info) = 0;
};

}
