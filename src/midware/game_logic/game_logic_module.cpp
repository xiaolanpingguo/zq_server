#include "game_logic_module.h"

#include "server/world_session.h"
#include "server/world_packet.h"

#include "baselib/message/game_db_account.pb.h"
#include "baselib/message/game_db_object.pb.h"
#include "baselib/base_code/random.h"
#include "baselib/kernel/config_module.h"

#include "dependencies/zlib/zlib.h"
#include "midware/cryptography/sha1.h"
#include "midware/cryptography/big_number.h"
#include "midware/cryptography/auth_crypt.h"

#include "entities/player/player.h"
#include "common/game_time.h"
#include "dbc_stores//dbc_stores.h"

#include "config_header/cfg_playercreateinfo.hpp"
#include "config_header/cfg_item_template.hpp"


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
	objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	playerMgrModule_ = libManager_->findModule<IPlayerMgrModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	configModule_->create<CSVItemTemplate>("cfg_item_template.csv");
	configModule_->create<CSVPlayerCreateInfo>("cfg_playercreateinfo.csv");

	const auto& all_row = configModule_->getCsvRowAll<CSVPlayerCreateInfo>();
	for (auto& ele : *all_row)
	{
		playerCreateInfo_[ele.second.race_id][ele.second.class_id] = (CSVPlayerCreateInfo*)&ele.second;
	}

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
	std::string hash_key = _KEY_ACCOUNT_ + authSession->Account;
	DBAccount::DBUserAccount account;
	if (!dataAgentModule_->getHashData(hash_key, authSession->Account, account))
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

	if (memcmp(sha.GetDigest(), authSession->Digest, SHA_DIGEST_LENGTH) != 0)
	{
		sendAuthResponseError(session, AUTH_FAILED);
		LOG_ERROR << fmt::format("Authentication failed for account: ('%s')", authSession->Account.c_str());
		session->delayedClose();
		return true;
	}

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
	session->setAccoutName(authSession->Account);

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
	const std::string& account_name = session->getAccoutName();
	auto redis_client = redisModule_->getClientBySuitConsistent();
	if (redis_client == nullptr)
	{
		return false;
	}

	WorldPacket data(SMSG_CHAR_ENUM, 100);                  // we guess size
	std::vector<string_pair> values;
	redis_client->HGETALL(_KEY_PLAYER_ + account_name, values);

	data << (uint8)values.size();
	for (const auto& ele : values)
	{
		DBObject::DBObjectInfo db_obj;
		if (!db_obj.ParseFromString(ele.second))
		{
			continue;
		}

		Player player(session);
		player.loadFromDb(db_obj);

		data << ObjectGuid(player.getValueGuid(EPlayerFieldUint64::PLAYER_GUID));
		data << player.getValueString(EPlayerFieldString::PLAYER_NAME);                         // name
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_RACE));                                // race
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_CLASS));                               // class
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_GENDER));    // gender
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_SKIN));
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_FACE));
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_HAIR_STYLE));
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_HAIR_COLOR));
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_FACIA_HAIR));
		data << uint8(player.getValueInt32(EPlayerFieldInt32::PLAYER_LEVEL));                   // level
		data << uint32(player.getValueInt32(EPlayerFieldInt32::PLAYER_ZONE_ID));                 // zone
		data << uint32(player.getValueInt32(EPlayerFieldInt32::PLAYER_MAP_ID));                 // map
		data << player.getValueFloat(EPlayerFieldFloat::PLAYER_POSITION_X);
		data << player.getValueFloat(EPlayerFieldFloat::PLAYER_POSITION_Y);
		data << player.getValueFloat(EPlayerFieldFloat::PLAYER_POSITION_Z);
		data << uint32(player.getValueInt32(EPlayerFieldInt32::PLAYER_TRANSPOAT_ID));                // transport guild id

		uint32 charFlags = 0;
		uint16 atLoginFlags = 0;
		//uint16 atLoginFlags = fields[18].GetUInt16();
		/*uint32 playerFlags = fields[17].GetUInt32();
		if (atLoginFlags & AT_LOGIN_RESURRECT)
			playerFlags &= ~PLAYER_FLAGS_GHOST;
		if (playerFlags & PLAYER_FLAGS_HIDE_HELM)
			charFlags |= CHARACTER_FLAG_HIDE_HELM;
		if (playerFlags & PLAYER_FLAGS_HIDE_CLOAK)
			charFlags |= CHARACTER_FLAG_HIDE_CLOAK;
		if (playerFlags & PLAYER_FLAGS_GHOST)
			charFlags |= CHARACTER_FLAG_GHOST;
		if (atLoginFlags & AT_LOGIN_RENAME)
			charFlags |= CHARACTER_FLAG_RENAME;
		if (fields[23].GetUInt32())
			charFlags |= CHARACTER_FLAG_LOCKED_BY_BILLING;
		if (sWorld->getBoolConfig(CONFIG_DECLINED_NAMES_USED))
		{
			if (!fields[24].GetString().empty())
				charFlags |= CHARACTER_FLAG_DECLINED;
		}
		else*/
		charFlags |= CHARACTER_FLAG_DECLINED;

		data << uint32(charFlags);                             // character flags

		// character customize flags
		//if (atLoginFlags & AT_LOGIN_CUSTOMIZE)
		//	data << uint32(CHAR_CUSTOMIZE_FLAG_CUSTOMIZE);
		//else if (atLoginFlags & AT_LOGIN_CHANGE_FACTION)
		//	data << uint32(CHAR_CUSTOMIZE_FLAG_FACTION);
		//else if (atLoginFlags & AT_LOGIN_CHANGE_RACE)
		//	data << uint32(CHAR_CUSTOMIZE_FLAG_RACE);
		//else
		data << uint32(CHAR_CUSTOMIZE_FLAG_NONE);

		// First login
		data << uint8(atLoginFlags & AT_LOGIN_FIRST ? 1 : 0);

		// Pets info
		uint32 petDisplayId = 0;
		uint32 petLevel = 0;
		CreatureFamily petFamily = CREATURE_FAMILY_NONE;
		data << uint32(petDisplayId);
		data << uint32(petLevel);
		data << uint32(petFamily);

		//Tokenizer equipment(fields[22].GetString(), ' ');
		for (uint8 slot = 0; slot < INVENTORY_SLOT_BAG_END; ++slot)
		{
			//uint32 visualBase = slot * 2;
			//uint32 itemId = GetUInt32ValueFromArray(equipment, visualBase);
			//ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
			//if (!proto)
			//{
			data << uint32(0);
			data << uint8(0);
			data << uint32(0);
			//continue;
		//}

		//SpellItemEnchantmentEntry const* enchant = nullptr;

		//uint32 enchants = GetUInt32ValueFromArray(equipment, visualBase + 1);
		//for (uint8 enchantSlot = PERM_ENCHANTMENT_SLOT; enchantSlot <= TEMP_ENCHANTMENT_SLOT; ++enchantSlot)
		//{
		//	// values stored in 2 uint16
		//	uint32 enchantId = 0x0000FFFF & (enchants >> enchantSlot * 16);
		//	if (!enchantId)
		//		continue;

		//	enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
		//	if (enchant)
		//		break;
		//}

		//data << uint32(proto->DisplayInfoID);
		//data << uint8(proto->InventoryType);
		//data << uint32(enchant ? enchant->aura_id : 0);
		//data << uint32(0);
		}
	}

	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleCharCreateOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	auto finiteAlways = [](float f) { return std::isfinite(f) ? f : 0.0f; };

	CharacterCreateInfo createInfo;

	recvPacket >> createInfo.Name
		>> createInfo.Race
		>> createInfo.Class
		>> createInfo.Gender
		>> createInfo.Skin
		>> createInfo.Face
		>> createInfo.HairStyle
		>> createInfo.HairColor
		>> createInfo.FacialHair
		>> createInfo.OutfitId;

	CSVPlayerCreateInfo const* info = getPlayerCreateInfo(createInfo.Race, createInfo.Class);
	if (!info)
	{
		LOG_ERROR << fmt::format("Player::Create: Possible hacking attempt: Account tried to create a character named {} with an invalid race/class pair ({}/{}) - refusing to do so.",
			session->getAccoutName(), createInfo.Race, createInfo.Class);
		return false;
	}

	uint8 resp_code = CHAR_CREATE_SUCCESS;
	do 
	{
		ObjectGuid guid = objectMgrModule_->createGuid(HighGuid::Player, 0);
		Player player(session);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_RACE, createInfo.Race);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_CLASS, createInfo.Class);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_GENDER, createInfo.Gender);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_SKIN, createInfo.Skin);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_FACE, createInfo.Face);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_HAIR_STYLE, createInfo.HairStyle);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_HAIR_COLOR, createInfo.HairColor);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_FACIA_HAIR, createInfo.FacialHair);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_OUT_FIT_ID, createInfo.OutfitId);
		player.setValueString(EPlayerFieldString::PLAYER_NAME, createInfo.Name);
		player.setValueGuid(EPlayerFieldUint64::PLAYER_GUID, guid);

		player.setValueInt32(EPlayerFieldInt32::PLAYER_LEVEL, info->level);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_ZONE_ID, info->zone_id);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_MAP_ID, info->map_id);
		player.setValueInt32(EPlayerFieldInt32::PLAYER_TRANSPOAT_ID, 0);

		player.setValueFloat(EPlayerFieldFloat::PLAYER_POSITION_X, info->position_x);
		player.setValueFloat(EPlayerFieldFloat::PLAYER_POSITION_Y, info->position_y);
		player.setValueFloat(EPlayerFieldFloat::PLAYER_POSITION_Z, info->position_z);

		if (CharStartOutfitEntry const* oEntry = GetCharStartOutfitEntry(createInfo.Race, createInfo.Class, createInfo.Gender))
		{
			for (int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
			{
				//uint32 itemId = oEntry->ItemId[j];

				//if (itemId <= 0)
				//	continue;

				//ItemTemplate const* iProto = objectMgrModule_->(itemId);
				//if (!iProto)
				//	continue;

				//// BuyCount by default
				//uint32 count = iProto->BuyCount;

				//// special amount for food/drink
				//if (iProto->Class == ITEM_CLASS_CONSUMABLE && iProto->SubClass == ITEM_SUBCLASS_FOOD)
				//{
				//	switch (iProto->Spells[0].SpellCategory)
				//	{
				//	case SPELL_CATEGORY_FOOD:                                // food
				//		count = getClass() == CLASS_DEATH_KNIGHT ? 10 : 4;
				//		break;
				//	case SPELL_CATEGORY_DRINK:                                // drink
				//		count = 2;
				//		break;
				//	}
				//	if (iProto->GetMaxStackSize() < count)
				//		count = iProto->GetMaxStackSize();
				//}
				//StoreNewItemInBestSlots(itemId, count);
			}
		}

		uint64 player_gid = kernelModule_->gen_uuid();
		DBObject::DBObjectInfo db_obj;
		player.saveDb(db_obj);
		dataAgentModule_->setHashData(_KEY_PLAYER_ + session->getAccoutName(), std::to_string(player_gid), db_obj);

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

CSVPlayerCreateInfo* GameLogicModule::getPlayerCreateInfo(int race_id, int class_id)
{
	if (race_id >= MAX_RACES || race_id <= 0)
		return nullptr;
	if (class_id >= MAX_CLASSES || class_id <= 0)
		return nullptr;

	return playerCreateInfo_[race_id][class_id];
}



}

