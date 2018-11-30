#include "world_session.h"
#include "server/game/game_cs_server/game_socket.h"
#include "world_packet.h"


#include "baselib/base_code/random.h"
#include "midware/cryptography/big_number.h"


namespace zq {


static std::vector<BannedAddon> s_bannedAddons;

WorldSession::WorldSession(std::shared_ptr<GameSocket> sock)
	:m_Socket(sock),
	accountId_(0),
	isAuth_(false),
	authSeed_(rand32())
{
	memset(m_Tutorials, 0, sizeof(uint32) * MAX_ACCOUNT_TUTORIAL_VALUES);
}

WorldSession::~WorldSession()
{
	if (m_Socket)
	{
		m_Socket->closeSocket();
		m_Socket = nullptr;
	}
}

void WorldSession::initializeSession()
{
	sendAuthResponse(AUTH_OK, true);
	sendAddonsInfo();
	sendClientCacheVersion(64);
	sendTutorialsData();
}

void WorldSession::sendAuthResponse(uint8 code, bool shortForm, uint32 queuePos)
{
	WorldPacket packet(SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4 + 1 + (shortForm ? 0 : (4 + 1)));
	packet << uint8(code);
	packet << uint32(0);                                   // BillingTimeRemaining
	packet << uint8(0);                                    // BillingPlanFlags
	packet << uint32(0);                                   // BillingTimeRested
	packet << uint8(2);                          // 0 - normal, 1 - TBC, 2 - WOTLK, must be set in database manually for each account

	if (!shortForm)
	{
		packet << uint32(queuePos);                             // Queue position
		packet << uint8(0);                                     // Realm has a free character migration - bool
	}

	sendPacket(packet);
}

void WorldSession::sendClientCacheVersion(uint32 version)
{
	WorldPacket data(SMSG_CLIENTCACHE_VERSION, 4);
	data << uint32(version);
	sendPacket(data);
}

void WorldSession::close()
{
	m_Socket->closeSocket();
}

void WorldSession::delayedClose()
{
	m_Socket->delayedCloseSocket();
}


void WorldSession::sendPacket(const WorldPacket& packet)
{
	EncryptablePacket encry_packet(packet, authCrypt_.IsInitialized());

	ServerPktHeader header(encry_packet.size() + 2, encry_packet.GetOpcode());
	if (encry_packet.NeedsEncryption())
	{
		authCrypt_.EncryptSend(header.header, header.getHeaderLength());
	}

	MessageBuffer buffer(encry_packet.size() + header.getHeaderLength());
	buffer.write(header.header, header.getHeaderLength());
	if (!encry_packet.empty())
	{
		buffer.write(encry_packet.contents(), encry_packet.size());
	}

	if (buffer.getActiveSize() > 0)
	{
		m_Socket->sendPacket(std::move(buffer));
	}
}

void WorldSession::addonesData(const AddonInfo& data)
{
	addonsList_.push_back(data);
}

void WorldSession::sendAddonsInfo()
{
	static const uint8 addonPublicKey[256] =
	{
		0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D, 0x54,
		0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45, 0x75,
		0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59, 0x34,
		0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28, 0xA8,
		0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C, 0xC8,
		0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F, 0x5A,
		0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2, 0xC3,
		0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46, 0xB9,
		0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD, 0x22,
		0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92, 0x7E,
		0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB, 0xFB,
		0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8, 0xF7,
		0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36, 0xF0,
		0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0, 0xA6,
		0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3, 0x9A,
		0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89, 0xD2
	};

	WorldPacket data(SMSG_ADDON_INFO, 4);

	for (auto itr = addonsList_.begin(); itr != addonsList_.end(); ++itr)
	{
		data << uint8(itr->State);

		uint8 crcpub = itr->UsePublicKeyOrCRC;
		data << uint8(crcpub);
		if (crcpub)
		{
			uint8 usepk = (itr->CRC != STANDARD_ADDON_CRC); // If addon is Standard addon CRC
			data << uint8(usepk);
			if (usepk)                                      // if CRC is wrong, add public key (client need it)
			{
				data.append(addonPublicKey, sizeof(addonPublicKey));
			}

			data << uint32(0);                              /// @todo Find out the meaning of this.
		}

		data << uint8(0);       // uses URL
								//if (usesURL)
								//    data << uint8(0); // URL
	}

	addonsList_.clear();

	data << uint32(s_bannedAddons.size());
	for (auto itr = s_bannedAddons.begin(); itr != s_bannedAddons.end(); ++itr)
	{
		data << uint32(itr->Id);
		data.append(itr->NameMD5, sizeof(itr->NameMD5));
		data.append(itr->VersionMD5, sizeof(itr->VersionMD5));
		data << uint32(itr->Timestamp);
		data << uint32(1);  // IsBanned
	}

	sendPacket(data);
}

void WorldSession::sendTutorialsData()
{
	WorldPacket data(SMSG_TUTORIAL_FLAGS, 4 * MAX_ACCOUNT_TUTORIAL_VALUES);
	for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
		data << m_Tutorials[i];

	sendPacket(data);
}

void WorldSession::sendNotification(const char *format, ...)
{
	if (format)
	{
		va_list ap;
		char szStr[1024];
		szStr[0] = '\0';
		va_start(ap, format);
		vsnprintf(szStr, 1024, format, ap);
		va_end(ap);

		WorldPacket data(SMSG_NOTIFICATION, (strlen(szStr) + 1));
		data << szStr;
		sendPacket(data);
	}
}

void WorldSession::Handle_NULL(WorldPacket& null)
{

}

void WorldSession::Handle_EarlyProccess(WorldPacket& recvPacket)
{

}

void WorldSession::Handle_ServerSide(WorldPacket& recvPacket)
{

}

void WorldSession::Handle_Deprecated(WorldPacket& recvPacket)
{

}

void WorldSession::handleSendAuthSession()
{
	WorldPacket packet(SMSG_AUTH_CHALLENGE, 37);
	packet << uint32(1);                                    // 1...31
	packet << uint32(authSeed_);

	BigNumber seed1;
	seed1.SetRand(16 * 8);
	packet.append(seed1.AsByteArray(16).get(), 16);               // new encryption seeds

	BigNumber seed2;
	seed2.SetRand(16 * 8);
	packet.append(seed2.AsByteArray(16).get(), 16);               // new encryption seeds

	sendPacket(packet);
}

}
