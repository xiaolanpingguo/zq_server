#include "game_logic_module.h"

#include "server/world_session.h"
#include "server/world_packet.h"
#include "baselib/message/config_define.hpp"
#include "baselib/message/game_db_account.pb.h"

#include "baselib/base_code/random.h"
#include "dependencies/zlib/zlib.h"
#include "midware/cryptography/sha1.h"
#include "midware/cryptography/big_number.h"
#include "midware/cryptography/auth_crypt.h"

#include "common/shared_defines.h"
#include "common/game_time.h"
#include "entities/object/object_guid.h"


namespace zq {


struct AuthSession
{
	uint32 BattlegroupID = 0;
	uint32 LoginServerType = 0;
	uint32 RealmID = 0;
	uint32 Build = 0;
	uint32 LocalChallenge = 0;
	uint32 LoginServerID = 0;
	uint32 RegionID = 0;
	uint64 DosResponse = 0;
	uint8 Digest[SHA_DIGEST_LENGTH] = {};
	std::string Account;
	ByteBuffer AddonInfo;
};


GameLogicModule::GameLogicModule(ILibManager* p)
{
	libManager_ = p;
}

GameLogicModule::~GameLogicModule()
{

}

bool GameLogicModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	redisModule_ = libManager_->findModule<IRedisModule>();
	dataAgentModule_ = libManager_->findModule<IDataAgentModule>();
	addonsModule_ = libManager_->findModule<IAddonsModule>();

	return true;
}

bool GameLogicModule::initEnd()
{
	using namespace std::placeholders;
	addMsgFun(CMSG_PING, std::bind(&GameLogicModule::handlePing, this, _1, _2));
	addMsgFun(CMSG_KEEP_ALIVE, std::bind(&GameLogicModule::handleKeepLive, this, _1, _2));
	addMsgFun(CMSG_AUTH_SESSION, std::bind(&GameLogicModule::handleAuthSession, this, _1, _2));
	addMsgFun(CMSG_READY_FOR_ACCOUNT_DATA_TIMES, std::bind(&GameLogicModule::handleReadyForAccountDataTimes, this, _1, _2));
	addMsgFun(CMSG_REALM_SPLIT, std::bind(&GameLogicModule::handleRealmSplitOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_ENUM, std::bind(&GameLogicModule::handleCharEnumOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_CREATE, std::bind(&GameLogicModule::handleCharCreateOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_DELETE, std::bind(&GameLogicModule::handleCharDeleteOpcode, this, _1, _2));

	return true;
}

bool GameLogicModule::run()
{
	return true;
}

bool GameLogicModule::loadFromDB()
{

	return true;
}

bool GameLogicModule::saveToDB()
{
	return true;
}

bool GameLogicModule::addMsgFun(int32 msgid, MsgCallBackFun&& fun)
{
	return msgFuns_.insert(std::make_pair(msgid, std::move(fun))).second;
}

void GameLogicModule::call(int32 msgid, WorldSession* session, WorldPacket& recvPacket)
{
	auto it = msgFuns_.find(msgid);
	if (it != msgFuns_.end())
	{
		(it->second)(session, recvPacket);
	}
	else
	{

	}
}

bool GameLogicModule::handleAuthSession(WorldSession* session, WorldPacket& recvPacket)
{
	if (session->isAuthed())
	{
		session->close();
		return false;
	}

	std::shared_ptr<AuthSession> authSession = std::make_shared<AuthSession>();
	recvPacket >> authSession->Build;
	recvPacket >> authSession->LoginServerID;
	recvPacket >> authSession->Account;
	recvPacket >> authSession->LoginServerType;
	recvPacket >> authSession->LocalChallenge;
	recvPacket >> authSession->RegionID;
	recvPacket >> authSession->BattlegroupID;
	recvPacket >> authSession->RealmID;               // realmId from auth_database.realmlist table
	recvPacket >> authSession->DosResponse;
	recvPacket.read(authSession->Digest, 20);
	authSession->AddonInfo.append(recvPacket.contents() + recvPacket.rpos(), recvPacket.size() - recvPacket.rpos());

	// 去数据库查找帐号信息
	std::string field_key = _KEY_ACCOUNT_ + authSession->Account;
	DBAccount::DBUserAccount account;
	if (!dataAgentModule_->getHashData(authSession->Account, field_key, account))
	{
		sendAuthResponseError(session, AUTH_UNKNOWN_ACCOUNT);
		session->delayedClose();
		LOG_ERROR << "login faild, name: " << authSession->Account;
		return true;
	}

	// session key
	BigNumber session_key;
	session_key.SetHexStr(account.session_key().c_str());
	session->getAuthCrypt().Init(&session_key);

	uint32 t = 0;

	uint32 auth_seed = session->getAuthSeed();
	SHA1Hash sha;
	sha.UpdateData(authSession->Account);
	sha.UpdateData((uint8*)&t, 4);
	sha.UpdateData((uint8*)&authSession->LocalChallenge, 4);
	sha.UpdateData((uint8*)&auth_seed, 4);
	sha.UpdateBigNumbers(&session_key, nullptr);
	sha.Finalize();

	/*if (memcmp(sha.GetDigest(), authSession->Digest, SHA_DIGEST_LENGTH) != 0)
	{
		sendAuthResponseError(session, AUTH_FAILED);
		LOG_ERROR << fmt::format("Authentication failed for account: ('%s')", authSession->Account.c_str());
		session->delayedClose();
		return true;
	}*/

	int64 mutetime = 0;
	//int64 mutetime = account.MuteTime;
	////! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
	//if (mutetime < 0)
	//{
	//	mutetime = GameTime::GetGameTime() + llabs(mutetime);

	//	stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
	//	stmt->setInt64(0, mutetime);
	//	stmt->setUInt32(1, account.Id);
	//	LoginDatabase.Execute(stmt);
	//}

	if (account.is_banned())
	{
		sendAuthResponseError(session, AUTH_BANNED);
		session->delayedClose();
		return true;
	}

	LOG_INFO << fmt::format("Client '%s' authenticated successfully.", authSession->Account.c_str());

	session->setAuthed(true);
	int32 Expansion = 2;
	static uint64 account_id = 0;
	int8 Locale = 4;
	int32 Recruiter = 0;
	bool IsRectuiter = false;
	session->setAuthed(auth_seed);

	// 读取附加数据
	readAddonsInfo(session, authSession->AddonInfo);
	session->initializeSession();

	account_id++;

	return true;
}

void GameLogicModule::readAddonsInfo(WorldSession* session, ByteBuffer& data)
{
	if (data.rpos() + 4 > data.size())
		return;

	uint32 size;
	data >> size;

	if (!size)
		return;

	if (size > 0xFFFFF)
	{
		LOG_ERROR << "AddOnInfo too big, size: " << size;
		return;
	}

	uLongf uSize = size;
	uint32 pos = data.rpos();

	ByteBuffer addonInfo;
	addonInfo.resize(size);

	if (uncompress(addonInfo.contents(), &uSize, data.contents() + pos, data.size() - pos) == Z_OK)
	{
		uint32 addonsCount;
		addonInfo >> addonsCount;

		for (uint32 i = 0; i < addonsCount; ++i)
		{
			std::string addonName;
			uint8 enabled;
			uint32 crc, unk1;

			// check next addon data format correctness
			if (addonInfo.rpos() + 1 > addonInfo.size())
				return;

			addonInfo >> addonName;
			addonInfo >> enabled >> crc >> unk1;

			AddonInfo addon(addonName, enabled, crc, 2, true);
			SavedAddon const* savedAddon = addonsModule_->getAddonInfo(addonName);
			if (savedAddon)
			{
				if (addon.CRC != savedAddon->CRC)
					LOG_WARN << fmt::format("Addon: %s: modified (CRC: 0x%x) - accountID:{})", addon.Name.c_str(), savedAddon->CRC, session->getAccountId());
			}
			else
			{
				addonsModule_->saveAddon(addon);
				LOG_WARN << fmt::format("Addon: %s: unknown (CRC: 0x%x) - accountId %d (storing addon name and checksum to database)", addon.Name.c_str(), addon.CRC, session->getAccountId());
			}

			// @todo Find out when to not use CRC/pubkey, and other possible states.
			session->addonesData(addon);
		}

		uint32 currentTime;
		addonInfo >> currentTime;
	}
	else
	{
		LOG_ERROR << "AddOn: Addon packet uncompress error!";
	}
}

bool GameLogicModule::handlePing(WorldSession* session, WorldPacket& recvPacket)
{
	using namespace std::chrono;

	uint32 ping;
	uint32 latency;

	// Get the ping packet content
	recvPacket >> ping;
	recvPacket >> latency;

	if (session->getLastPingTime() == steady_clock::time_point())
	{
		session->setLastPingTime(steady_clock::now());
	}
	else
	{
		steady_clock::time_point now = steady_clock::now();
		steady_clock::duration diff = now - session->getLastPingTime();
		session->setLastPingTime(now);
	}

	//if (_worldSession)
	//{
	//	_worldSession->SetLatency(latency);
	//	_worldSession->ResetClientTimeDelay();
	//}
	//else
	//{
	//	LOG_ERROR << fmt::format("peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", getIp().c_str());
	//	return false;
	//}

	WorldPacket packet(SMSG_PONG, 4);
	packet << ping;
	session->sendPacket(packet);

	return true;
}

bool GameLogicModule::handleKeepLive(WorldSession* session, WorldPacket& recvPacket)
{
	return true;
}

bool GameLogicModule::handleReadyForAccountDataTimes(WorldSession* session, WorldPacket& recvPacket)
{
	static const int mask = 0x15;
	WorldPacket data(SMSG_ACCOUNT_DATA_TIMES, 4 + 1 + 4 + NUM_ACCOUNT_DATA_TYPES * 4);
	data << uint32(GameTime::GetGameTime());                             // Server time
	data << uint8(1);
	data << uint32(mask);                                   // type mask
	for (uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
		if (mask & (1 << i))
			data << uint32(session->GetAccountData(AccountDataType(i))->Time);// also unix time

	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleRealmSplitOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	uint32 unk;
	std::string split_date = "01/01/01";
	recvPacket >> unk;

	WorldPacket data(SMSG_REALM_SPLIT, 4 + 4 + split_date.size() + 1);
	data << unk;
	data << uint32(0x00000000);                             // realm split state
	// split states:
	// 0x0 realm normal
	// 0x1 realm split
	// 0x2 realm split pending
	data << split_date;
	session->sendPacket(data);
	return true;
}

bool GameLogicModule::handleCharEnumOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	WorldPacket data(SMSG_CHAR_ENUM, 100);                  // we guess size
	uint8 num = 0;
	data << num;
	data.put<uint8>(0, num);
	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleCharCreateOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	std::shared_ptr<CharacterCreateInfo> createInfo = std::make_shared<CharacterCreateInfo>();

	recvPacket >> createInfo->Name
		>> createInfo->Race
		>> createInfo->Class
		>> createInfo->Gender
		>> createInfo->Skin
		>> createInfo->Face
		>> createInfo->HairStyle
		>> createInfo->HairColor
		>> createInfo->FacialHair
		>> createInfo->OutfitId;

	uint8 resp_code = CHAR_CREATE_SUCCESS;
	do 
	{
		//// 名字重复
		//resp_code = CHAR_CREATE_NAME_IN_USE;

		//// 角色限制
		//resp_code = CHAR_CREATE_NAME_IN_USE;
		//// 错误
		//resp_code = CHAR_CREATE_ERROR;
	} while (0);

	WorldPacket data(SMSG_CHAR_CREATE, 1);
	data << uint8(resp_code);
	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleCharDeleteOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	return true;
}

void GameLogicModule::sendAuthResponseError(WorldSession* session, uint8 code)
{
	WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
	packet << uint8(code);

	session->sendPacket(packet);
}

void GameLogicModule::onWorldSessionInit(WorldSession* session)
{

}


}

