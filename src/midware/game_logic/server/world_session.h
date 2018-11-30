#pragma once 


#include "interface_header/base/platform.h"
#include "midware/cryptography/auth_crypt.h"
#include "interface_header/logic/IAddonsModule.h"


namespace zq {


#define MAX_ACCOUNT_TUTORIAL_VALUES 8
#define NUM_ACCOUNT_DATA_TYPES        8

#define GLOBAL_CACHE_MASK           0x15
#define PER_CHARACTER_CACHE_MASK    0xEA

enum AccountDataType
{
	GLOBAL_CONFIG_CACHE = 0,                    // 0x01 g
	PER_CHARACTER_CONFIG_CACHE = 1,                    // 0x02 p
	GLOBAL_BINDINGS_CACHE = 2,                    // 0x04 g
	PER_CHARACTER_BINDINGS_CACHE = 3,                    // 0x08 p
	GLOBAL_MACROS_CACHE = 4,                    // 0x10 g
	PER_CHARACTER_MACROS_CACHE = 5,                    // 0x20 p
	PER_CHARACTER_LAYOUT_CACHE = 6,                    // 0x40 p
	PER_CHARACTER_CHAT_CACHE = 7                     // 0x80 p
};

enum Gender
{
	GENDER_MALE = 0,
	GENDER_FEMALE = 1,
	GENDER_NONE = 2
};

// 创角数据
struct CharacterCreateInfo
{

	std::string Name;
	uint8 Race = 0;
	uint8 Class = 0;
	uint8 Gender = GENDER_NONE;   
	uint8 Skin = 0;
	uint8 Face = 0;
	uint8 HairStyle = 0;
	uint8 HairColor = 0;
	uint8 FacialHair = 0;
	uint8 OutfitId = 0;

	// Server side data
	uint8 CharCount = 0;
};

struct AccountData
{
	AccountData() : Time(0), Data("") { }

	time_t Time;
	std::string Data;
};

class GameSocket;
class WorldPacket;
class ByteBuffer;
class WorldSession
{
public:
	WorldSession(std::shared_ptr<GameSocket> sock);
	~WorldSession();

public:

	void initializeSession();
	void close();
	void delayedClose();

public:

	void sendPacket(const WorldPacket& packet);
	void sendAuthResponse(uint8 code, bool shortForm, uint32 queuePos = 0);
	void sendClientCacheVersion(uint32 version);
	void addonesData(const AddonInfo& data);
	void sendAddonsInfo();
	void sendTutorialsData();

	void sendNotification(const char *format, ...);
public:

	void handleSendAuthSession();
	void Handle_NULL(WorldPacket& recvPacket);          // not used
	void Handle_EarlyProccess(WorldPacket& recvPacket); // just mark packets processed in WorldSocket::OnRead
	void Handle_ServerSide(WorldPacket& recvPacket);    // sever side only, can't be accepted from client
	void Handle_Deprecated(WorldPacket& recvPacket);    // never used anymore by client

public:


public:

	bool isAuthed() { return isAuth_; }
	void setAuthed(bool v) { isAuth_ = v; }

	uint64 getAccountId() { return accountId_; }

	AuthCrypt& getAuthCrypt() { return authCrypt_; }

	uint32 getAuthSeed() { return authSeed_; }
	void setAuthSeed(uint32 v) { authSeed_ = v; }

	const auto& getLastPingTime() { return lastPingTime_; }
	void setLastPingTime(const std::chrono::steady_clock::time_point& time) { lastPingTime_ = time; }

	AccountData* GetAccountData(AccountDataType type) { return &m_accountData[type]; }

private:

	std::shared_ptr<GameSocket> m_Socket;

	uint64 accountId_;
	bool isAuth_;
	uint32 authSeed_;
	AuthCrypt authCrypt_;
	std::chrono::steady_clock::time_point lastPingTime_;	//上次ping time

	// --------------DB---------
	AddonsList addonsList_;	// 附加数据
	AccountData m_accountData[NUM_ACCOUNT_DATA_TYPES];
	uint32 m_Tutorials[MAX_ACCOUNT_TUTORIAL_VALUES];
};

}
