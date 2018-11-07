#include "login_cs_session.h"
#include "baselib/base_code/util.h"
#include "baselib/network/byte_buffer.hpp"
#include "baselib/message/game_db_account.pb.h"
#include "midware/cryptography/sha1.h"
#include "midware/cryptography/totp.h"
#include "baselib/libloader/libmanager.h"
#include "interface_header/IDataAgentModule.h"
#include <unordered_map>
#include <array>

namespace zq {

static RealmBuildInfo const PostBcAcceptedClientBuilds[] =
{
	{ 15595, 4, 3, 4, ' ' },
	{ 14545, 4, 2, 2, ' ' },
	{ 13623, 4, 0, 6, 'a' },
	{ 13930, 3, 3, 5, 'a' },                                  // 3.3.5a China Mainland build
	{ 13507, 3, 3, 5, 'a' },
	{ 12340, 3, 3, 5, 'a' },
	{ 11723, 3, 3, 3, 'a' },
	{ 11403, 3, 3, 2, ' ' },
	{ 11159, 3, 3, 0, 'a' },
	{ 10505, 3, 2, 2, 'a' },
	{ 9947,  3, 1, 3, ' ' },
	{ 8606,  2, 4, 3, ' ' },
	{ 0,     0, 0, 0, ' ' }                                   // terminator
};

static RealmBuildInfo const PreBcAcceptedClientBuilds[] =
{
	{ 6141,  1, 12, 3, ' ' },
	{ 6005,  1, 12, 2, ' ' },
	{ 5875,  1, 12, 1, ' ' },
	{ 0,     0, 0, 0, ' ' }                                   // terminator
};


#pragma pack(push, 1)

struct AuthHandler
{
	AuthStatus status;
	size_t packetSize;
	bool (LoginCSSession::*handler)();
};

#pragma pack(pop)



enum eAuthCmd
{
    AUTH_LOGON_CHALLENGE = 0x00,
    AUTH_LOGON_PROOF = 0x01,
    AUTH_RECONNECT_CHALLENGE = 0x02,
    AUTH_RECONNECT_PROOF = 0x03,
    REALM_LIST = 0x10,
    XFER_INITIATE = 0x30,
    XFER_DATA = 0x31,
    XFER_ACCEPT = 0x32,
    XFER_RESUME = 0x33,
    XFER_CANCEL = 0x34
};

#pragma pack(push, 1)

typedef struct AUTH_LOGON_CHALLENGE_C
{
    uint8   cmd;
    uint8   error;
    uint16  size;
    uint8   gamename[4];
    uint8   version1;
    uint8   version2;
    uint8   version3;
    uint16  build;
    uint8   platform[4];
    uint8   os[4];
    uint8   country[4];
    uint32  timezone_bias;
    uint32  ip;
    uint8   I_len;
    uint8   I[1];
} sAuthLogonChallenge_C;

typedef struct AUTH_LOGON_PROOF_C
{
    uint8   cmd;
    uint8   A[32];
    uint8   M1[20];
    uint8   crc_hash[20];
    uint8   number_of_keys;
    uint8   securityFlags;
} sAuthLogonProof_C;

typedef struct AUTH_LOGON_PROOF_S
{
    uint8   cmd;
    uint8   error;
    uint8   M2[20];
    uint32  AccountFlags;
    uint32  SurveyId;
    uint16  unk3;
} sAuthLogonProof_S;

typedef struct AUTH_LOGON_PROOF_S_OLD
{
    uint8   cmd;
    uint8   error;
    uint8   M2[20];
    uint32  unk2;
} sAuthLogonProof_S_Old;

typedef struct AUTH_RECONNECT_PROOF_C
{
    uint8   cmd;
    uint8   R1[16];
    uint8   R2[20];
    uint8   R3[20];
    uint8   number_of_keys;
} sAuthReconnectProof_C;

#pragma pack(pop)

enum class BufferSizes : uint32
{
    SRP_6_V = 0x20,
    SRP_6_S = 0x20,
};

#define MAX_ACCEPTED_CHALLENGE_SIZE (sizeof(AUTH_LOGON_CHALLENGE_C) + 16)

#define AUTH_LOGON_CHALLENGE_INITIAL_SIZE 4
#define REALM_LIST_PACKET_SIZE 5



static std::unordered_map<uint8, AuthHandler> const s_handlers = 
{
	{AUTH_LOGON_CHALLENGE,		{STATUS_CHALLENGE, AUTH_LOGON_CHALLENGE_INITIAL_SIZE, &LoginCSSession::HandleLogonChallenge }},
	{AUTH_LOGON_PROOF,			{ STATUS_LOGON_PROOF, sizeof(AUTH_LOGON_PROOF_C),        &LoginCSSession::HandleLogonProof }},
	{AUTH_RECONNECT_CHALLENGE,	{ STATUS_CHALLENGE, AUTH_LOGON_CHALLENGE_INITIAL_SIZE, &LoginCSSession::HandleReconnectChallenge }},
	{AUTH_RECONNECT_PROOF,		{ STATUS_RECONNECT_PROOF, sizeof(AUTH_RECONNECT_PROOF_C),    &LoginCSSession::HandleReconnectProof }},
	{REALM_LIST,				{ STATUS_AUTHED,    REALM_LIST_PACKET_SIZE,            &LoginCSSession::HandleRealmList }},
};

LoginCSSession::LoginCSSession(tcp::socket&& socket) : Socket(std::move(socket)),
_status(STATUS_CHALLENGE), _build(0), _expversion(0)
{
    N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
    g.SetDword(7);

	int num = 3;
	for (int i = 0; i < num; ++i)
	{
		GameSvrInfoPtr svr_info = std::make_unique<GameSvrInfo>();
		svr_info->allowed_security_level = SEC_PLAYER;
		svr_info->build = 13507;
		svr_info->flag = REALM_FLAG_NONE;
		svr_info->icon = 0;
		svr_info->populationLevel = 0.3f;
		svr_info->time_zone = 1;
		svr_info->ip = "127.0.0.1";
		svr_info->port = 8035;
		svr_info->name = "zq";
		gameSvrList_.emplace_back(std::move(svr_info));
	}
}

void LoginCSSession::start()
{

}

bool LoginCSSession::update()
{
    if (!BaseSocket::update())
        return false;

    return true;
}

void LoginCSSession::readHandler()
{
	//ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
	//OpcodeClient opcode = static_cast<OpcodeClient>(header->cmd);

	//WorldPacket packet(opcode, std::move(_packetBuffer));
	//_worldSession->QueuePacket(new WorldPacket(std::move(packet)));

    MessageBuffer& packet = getReadBuffer();
    while (packet.getActiveSize())
    {
        uint8 cmd = packet.getReadPointer()[0];
        auto itr = s_handlers.find(cmd);
        if (itr == s_handlers.end())
        {
            // well we dont handle this, lets just ignore it
            packet.reset();
            break;
        }

        if (_status != itr->second.status)
        {
            closeSocket();
            return;
        }

        uint16 size = uint16(itr->second.packetSize);
        if (packet.getActiveSize() < size)
            break;

        if (cmd == AUTH_LOGON_CHALLENGE || cmd == AUTH_RECONNECT_CHALLENGE)
        {
            sAuthLogonChallenge_C* challenge = reinterpret_cast<sAuthLogonChallenge_C*>(packet.getReadPointer());
            size += challenge->size;
            if (size > MAX_ACCEPTED_CHALLENGE_SIZE)
            {
				closeSocket();
                return;
            }
        }

        if (packet.getActiveSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            closeSocket();
            return;
        }

        packet.readCompleted(size);
    }

    asyncRead();
}

void LoginCSSession::SendPacket(ByteBuffer& packet)
{
    if (!isOpen())
        return;

    if (!packet.empty())
    {
        MessageBuffer buffer;
        buffer.write(packet.contents(), packet.size());
        queuePacket(std::move(buffer));
    }
}

bool LoginCSSession::HandleLogonChallenge()
{
	//IDataAgentModule* data_agent = LibManager::get_instance().findModule<IDataAgentModule>();

    _status = STATUS_CLOSED;

    sAuthLogonChallenge_C* challenge = reinterpret_cast<sAuthLogonChallenge_C*>(getReadBuffer().getReadPointer());
    if (challenge->size - (sizeof(sAuthLogonChallenge_C) - AUTH_LOGON_CHALLENGE_INITIAL_SIZE - 1) != challenge->I_len)
        return false;

	uint8 error_code = WOW_SUCCESS;
	ByteBuffer pkt;
	pkt << uint8(AUTH_LOGON_CHALLENGE);
	pkt << uint8(0x00);

	// 检查版本
	if (!IsAcceptedClientBuild(challenge->build))
	{
		pkt << WOW_FAIL_VERSION_INVALID;
		SendPacket(pkt);
		return false;
	}
	else
	{
		pkt << uint8(WOW_SUCCESS);
	}

	std::string login_name((char const*)challenge->I, challenge->I_len);

	// 去数据库查找帐号信息
	std::string key = KEY_ACCOUNT + login_name;
	DBAccount::DBUserAccount account_info;
	//if (!data_agent->getData(login_name, key, account_info))
	{
		LOG_ERROR << "login faild, name: " << login_name;
		error_code = WOW_FAIL_UNKNOWN_ACCOUNT;
	}

	// 是否被封号
	if (account_info.is_banned())
	{
		error_code = WOW_FAIL_BANNED;
	}

	if (error_code != WOW_SUCCESS)
	{
		pkt << error_code;
		SendPacket(pkt);
		return false;
	}

	// 版本
    _build = challenge->build;
    _expversion = uint8(IsPostBCAcceptedClientBuild(_build) ? POST_BC_EXP_FLAG : 
		(IsPreBCAcceptedClientBuild(_build) ? PRE_BC_EXP_FLAG : NO_VALID_EXP_FLAG));

	// 系统
    std::array<char, 5> os;
    os.fill('\0');
    memcpy(os.data(), challenge->os, sizeof(challenge->os));
    _os = os.data();
    std::reverse(_os.begin(), _os.end());

	// 地区
    _localizationName.resize(4);
	for (int i = 0; i < 4; ++i)
	{
		_localizationName[i] = challenge->country[4 - i - 1];
	}

	std::string databaseV = account_info.v();
	std::string databaseS = account_info.s();
	std::string rI = account_info.password_hash();
	if (databaseV.size() != size_t(BufferSizes::SRP_6_V) * 2 || databaseS.size() != size_t(BufferSizes::SRP_6_S) * 2)
	{
		s.SetRand(int32(BufferSizes::SRP_6_S) * 8);

		BigNumber I;
		I.SetHexStr(rI.c_str());

		// In case of leading zeros in the rI hash, restore them
		uint8 mDigest[SHA_DIGEST_LENGTH];
		memcpy(mDigest, I.AsByteArray(SHA_DIGEST_LENGTH).get(), SHA_DIGEST_LENGTH);
		std::reverse(mDigest, mDigest + SHA_DIGEST_LENGTH);

		SHA1Hash sha;
		sha.UpdateData(s.AsByteArray(uint32(BufferSizes::SRP_6_S)).get(), (uint32(BufferSizes::SRP_6_S)));
		sha.UpdateData(mDigest, SHA_DIGEST_LENGTH);
		sha.Finalize();
		BigNumber x;
		x.SetBinary(sha.GetDigest(), sha.GetLength());
		v = g.ModExp(x, N);

		account_info.set_v(v.AsHexStr());
		account_info.set_s(s.AsHexStr());
	}
	else
	{
		s.SetHexStr(databaseS.c_str());
		v.SetHexStr(databaseV.c_str());
	}

	b.SetRand(19 * 8);
	BigNumber gmod = g.ModExp(b, N);
	B = ((v * 3) + gmod) % N;

	ASSERT(gmod.GetNumBytes() <= 32);

	BigNumber unk3;
	unk3.SetRand(16 * 8);

	if (IsAcceptedClientBuild(_build))
	{
		pkt << uint8(WOW_SUCCESS);
		_status = STATUS_LOGON_PROOF;
	}
	else
	{
		pkt << uint8(WOW_FAIL_VERSION_INVALID);
	}

	// B may be calculated < 32B so we force minimal length to 32B
	pkt.append(B.AsByteArray(32).get(), 32);      // 32 bytes
	pkt << uint8(1);
	pkt.append(g.AsByteArray(1).get(), 1);
	pkt << uint8(32);
	pkt.append(N.AsByteArray(32).get(), 32);
	pkt.append(s.AsByteArray(int32(BufferSizes::SRP_6_S)).get(), size_t(BufferSizes::SRP_6_S));   // 32 bytes
	pkt.append(unk3.AsByteArray(16).get(), 16);
	uint8 securityFlags = 0;

	// Check if token is used
	std::string tokenKey_ = "";
	if (!tokenKey_.empty())
		securityFlags = 4;

	pkt << uint8(securityFlags);            // security flags (0x0...0x04)

	if (securityFlags & 0x01)               // PIN input
	{
		pkt << uint32(0);
		pkt << uint64(0) << uint64(0);      // 16 bytes hash?
	}

	if (securityFlags & 0x02)               // Matrix input
	{
		pkt << uint8(0);
		pkt << uint8(0);
		pkt << uint8(0);
		pkt << uint8(0);
		pkt << uint64(0);
	}

	if (securityFlags & 0x04)               // Security token input
		pkt << uint8(1);

	SendPacket(pkt);
    return true;
}

bool LoginCSSession::HandleLogonProof()
{
    _status = STATUS_CLOSED;

    sAuthLogonProof_C *logonProof = reinterpret_cast<sAuthLogonProof_C*>(getReadBuffer().getReadPointer());

    // If the client has no valid version
    if (_expversion == NO_VALID_EXP_FLAG)
    {
        return false;
    }

    // Continue the SRP6 calculation based on data received from the client
    BigNumber A;
    A.SetBinary(logonProof->A, 32);

    // SRP safeguard: abort if A == 0
    if ((A % N).IsZero())
        return false;

    SHA1Hash sha;
    sha.UpdateBigNumbers(&A, &B, nullptr);
    sha.Finalize();
    BigNumber u;
    u.SetBinary(sha.GetDigest(), 20);
    BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);

    uint8 t[32];
    uint8 t1[16];
    uint8 vK[40];
    memcpy(t, S.AsByteArray(32).get(), 32);

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2];

    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        vK[i * 2] = sha.GetDigest()[i];

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2 + 1];

    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        vK[i * 2 + 1] = sha.GetDigest()[i];

    K.SetBinary(vK, 40);

    uint8 hash[20];

    sha.Initialize();
    sha.UpdateBigNumbers(&N, nullptr);
    sha.Finalize();
    memcpy(hash, sha.GetDigest(), 20);
    sha.Initialize();
    sha.UpdateBigNumbers(&g, nullptr);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        hash[i] ^= sha.GetDigest()[i];

    BigNumber t3;
    t3.SetBinary(hash, 20);

    sha.Initialize();
    sha.UpdateData(_accountInfo.Login);
    sha.Finalize();
    uint8 t4[SHA_DIGEST_LENGTH];
    memcpy(t4, sha.GetDigest(), SHA_DIGEST_LENGTH);

    sha.Initialize();
    sha.UpdateBigNumbers(&t3, nullptr);
    sha.UpdateData(t4, SHA_DIGEST_LENGTH);
    sha.UpdateBigNumbers(&s, &A, &B, &K, nullptr);
    sha.Finalize();
    BigNumber M;
    M.SetBinary(sha.GetDigest(), sha.GetLength());

    // Check if SRP6 results match (password is correct), else send an error
    if (!memcmp(M.AsByteArray(sha.GetLength()).get(), logonProof->M1, 20))
    {
        // Check auth token
        if ((logonProof->securityFlags & 0x04) || !_tokenKey.empty())
        {
            uint8 size = *(getReadBuffer().getReadPointer() + sizeof(sAuthLogonProof_C));
            std::string token(reinterpret_cast<char*>(getReadBuffer().getReadPointer() + sizeof(sAuthLogonProof_C) + sizeof(size)), size);
			getReadBuffer().readCompleted(sizeof(size) + size);
            uint32 validToken = TOTP::generateToken(_tokenKey.c_str());
            _tokenKey.clear();
            uint32 incomingToken = atoi(token.c_str());
            if (validToken != incomingToken)
            {
                ByteBuffer packet;
                packet << uint8(AUTH_LOGON_PROOF);
                packet << uint8(WOW_FAIL_UNKNOWN_ACCOUNT);
                packet << uint8(3);
                packet << uint8(0);
                SendPacket(packet);
                return true;
            }
        }

        // Update the sessionkey, last_ip, last login time and reset number of failed logins in the account table for this account
        // No SQL injection (escaped user name) and IP address as received by socket

        //PreparedStatement *stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGONPROOF);
        //stmt->setString(0, K.AsHexStr());
        //stmt->setString(1, GetRemoteIpAddress().to_string());
        //stmt->setUInt32(2, GetLocaleByName(_localizationName));
        //stmt->setString(3, _os);
        //stmt->setString(4, _accountInfo.Login);
        //LoginDatabase.DirectExecute(stmt);

        // Finish SRP6 and send the final result to the client
        sha.Initialize();
        sha.UpdateBigNumbers(&A, &M, &K, nullptr);
        sha.Finalize();

        ByteBuffer packet;
        if (_expversion & POST_BC_EXP_FLAG)                 // 2.x and 3.x clients
        {
            sAuthLogonProof_S proof;
            memcpy(proof.M2, sha.GetDigest(), 20);
            proof.cmd = AUTH_LOGON_PROOF;
            proof.error = 0;
            proof.AccountFlags = 0x00800000;    // 0x01 = GM, 0x08 = Trial, 0x00800000 = Pro pass (arena tournament)
            proof.SurveyId = 0;
            proof.unk3 = 0;

            packet.resize(sizeof(proof));
            std::memcpy(packet.contents(), &proof, sizeof(proof));
        }
        else
        {
            sAuthLogonProof_S_Old proof;
            memcpy(proof.M2, sha.GetDigest(), 20);
            proof.cmd = AUTH_LOGON_PROOF;
            proof.error = 0;
            proof.unk2 = 0x00;

            packet.resize(sizeof(proof));
            std::memcpy(packet.contents(), &proof, sizeof(proof));
        }

        SendPacket(packet);
        _status = STATUS_AUTHED;
    }
    else
    {
        ByteBuffer packet;
        packet << uint8(AUTH_LOGON_PROOF);
        packet << uint8(WOW_FAIL_UNKNOWN_ACCOUNT);
        packet << uint8(3);
        packet << uint8(0);
        SendPacket(packet);
    }

    return true;
}

bool LoginCSSession::HandleReconnectChallenge()
{
    _status = STATUS_CLOSED;

    sAuthLogonChallenge_C* challenge = reinterpret_cast<sAuthLogonChallenge_C*>(getReadBuffer().getReadPointer());
    if (challenge->size - (sizeof(sAuthLogonChallenge_C) - AUTH_LOGON_CHALLENGE_INITIAL_SIZE - 1) != challenge->I_len)
        return false;

    std::string login((char const*)challenge->I, challenge->I_len);

    _build = challenge->build;
    _expversion = uint8(IsPostBCAcceptedClientBuild(_build) ? POST_BC_EXP_FLAG : 
		(IsPreBCAcceptedClientBuild(_build) ? PRE_BC_EXP_FLAG : NO_VALID_EXP_FLAG));
    std::array<char, 5> os;
    os.fill('\0');
    memcpy(os.data(), challenge->os, sizeof(challenge->os));
    _os = os.data();
    std::reverse(_os.begin(), _os.end());

    _localizationName.resize(4);
    for (int i = 0; i < 4; ++i)
        _localizationName[i] = challenge->country[4 - i - 1];

	ByteBuffer pkt;
	pkt << uint8(AUTH_RECONNECT_CHALLENGE);

	//if (!result)
	//{
	//	pkt << uint8(WOW_FAIL_UNKNOWN_ACCOUNT);
	//	SendPacket(pkt);
	//	return;
	//}

	//Field* fields = result->Fetch();

	//_accountInfo.LoadResult(fields);
	//K.SetHexStr(fields[9].GetCString());
	//_reconnectProof.SetRand(16 * 8);
	//_status = STATUS_RECONNECT_PROOF;

	//pkt << uint8(WOW_SUCCESS);
	//pkt.append(_reconnectProof.AsByteArray(16).get(), 16);  // 16 bytes random
	//pkt << uint64(0x00) << uint64(0x00);                    // 16 bytes zeros

	SendPacket(pkt);

    return true;
}

bool LoginCSSession::HandleReconnectProof()
{
    _status = STATUS_CLOSED;

    sAuthReconnectProof_C *reconnectProof = reinterpret_cast<sAuthReconnectProof_C*>(getReadBuffer().getReadPointer());

    if (_accountInfo.Login.empty() || !_reconnectProof.GetNumBytes() || !K.GetNumBytes())
        return false;

    BigNumber t1;
    t1.SetBinary(reconnectProof->R1, 16);

    SHA1Hash sha;
    sha.Initialize();
    sha.UpdateData(_accountInfo.Login);
    sha.UpdateBigNumbers(&t1, &_reconnectProof, &K, nullptr);
    sha.Finalize();

    if (!memcmp(sha.GetDigest(), reconnectProof->R2, SHA_DIGEST_LENGTH))
    {
        // Sending response
        ByteBuffer pkt;
        pkt << uint8(AUTH_RECONNECT_PROOF);
        pkt << uint8(0x00);
        pkt << uint16(0x00);                               // 2 bytes zeros
        SendPacket(pkt);
        _status = STATUS_AUTHED;
        return true;
    }
    else
    {
        return false;
    }
}

bool LoginCSSession::HandleRealmList()
{
	ByteBuffer pkt;

	size_t RealmListSize = gameSvrList_.size();
	for (size_t i = 0; i < RealmListSize; ++i)
	{
		const GameSvrInfo& svr_info = *gameSvrList_[i];

		// don't work with realms which not compatible with the client
		bool okBuild = ((_expversion & POST_BC_EXP_FLAG) && svr_info.build == _build) ||
			((_expversion & PRE_BC_EXP_FLAG) && !IsPreBCAcceptedClientBuild(svr_info.build));

		// No SQL injection. id of realm is controlled by the database.
		uint32 flag = svr_info.flag;
		RealmBuildInfo const* buildInfo = GetBuildInfo(svr_info.build);
		if (!okBuild)
		{
			if (!buildInfo)
				continue;

			flag |= REALM_FLAG_OFFLINE | REALM_FLAG_SPECIFYBUILD;   // tell the client what build the realm is for
		}

		if (!buildInfo)
			flag &= ~REALM_FLAG_SPECIFYBUILD;

		std::string name = svr_info.name;
		if (_expversion & PRE_BC_EXP_FLAG && flag & REALM_FLAG_SPECIFYBUILD)
		{
			std::ostringstream ss;
			ss << name << " (" << buildInfo->MajorVersion << '.' << buildInfo->MinorVersion << '.' << buildInfo->BugfixVersion << ')';
			name = ss.str();
		}

		//uint8 lock = (svr_info.allowed_security_level > pending_info->acc.SecurityLevel) ? 1 : 0;
		uint8 lock = 0;

		pkt << uint8(svr_info.icon);                           // realm type
		if (_expversion & POST_BC_EXP_FLAG)                 // only 2.x and 3.x clients
			pkt << uint8(lock);                             // if 1, then realm locked
		pkt << uint8(flag);                                 // RealmFlags
		pkt << name;
		pkt << (svr_info.ip + ":" + std::to_string(svr_info.port));
		pkt << float(svr_info.populationLevel);
		pkt << uint8(1);		//角色数量
		pkt << uint8(svr_info.time_zone);                       // realm category，这个错误就会报客户端语言与版本不符
		if (_expversion & POST_BC_EXP_FLAG)                 // 2.x and 3.x clients
			pkt << uint8(i);			 // realmlist的table id
		else
			pkt << uint8(0x0);                              // 1.12.1 and 1.12.2 clients

		if (_expversion & POST_BC_EXP_FLAG && flag & REALM_FLAG_SPECIFYBUILD)
		{
			pkt << uint8(buildInfo->MajorVersion);
			pkt << uint8(buildInfo->MinorVersion);
			pkt << uint8(buildInfo->BugfixVersion);
			pkt << uint16(buildInfo->Build);
		}
	}

	if (_expversion & POST_BC_EXP_FLAG)                     // 2.x and 3.x clients
	{
		pkt << uint8(0x10);
		pkt << uint8(0x00);
	}
	else                                                    // 1.12.1 and 1.12.2 clients
	{
		pkt << uint8(0x00);
		pkt << uint8(0x02);
	}

	// make a ByteBuffer which stores the RealmList's size
	ByteBuffer RealmListSizeBuffer;
	RealmListSizeBuffer << uint32(0);
	if (_expversion & POST_BC_EXP_FLAG)                     // only 2.x and 3.x clients
		RealmListSizeBuffer << uint16(RealmListSize);
	else
		RealmListSizeBuffer << uint32(RealmListSize);

	ByteBuffer hdr;
	hdr << uint8(REALM_LIST);
	hdr << uint16(pkt.size() + RealmListSizeBuffer.size());
	hdr.append(RealmListSizeBuffer);                        // append RealmList's size buffer
	hdr.append(pkt);                                        // append realms in the realmlist
	SendPacket(hdr);

	_status = STATUS_AUTHED;

	return true;
}

bool LoginCSSession::IsPreBCAcceptedClientBuild(int build)
{
	for (int i = 0; PreBcAcceptedClientBuilds[i].Build; ++i)
		if (PreBcAcceptedClientBuilds[i].Build == build)
			return true;

	return false;
}

bool LoginCSSession::IsPostBCAcceptedClientBuild(int build)
{
	for (int i = 0; PostBcAcceptedClientBuilds[i].Build; ++i)
		if (PostBcAcceptedClientBuilds[i].Build == build)
			return true;

	return false;
}

bool LoginCSSession::IsAcceptedClientBuild(int build)
{
	return (IsPostBCAcceptedClientBuild(build) || IsPreBCAcceptedClientBuild(build));
}

RealmBuildInfo const* LoginCSSession::GetBuildInfo(int build)
{
	for (int i = 0; PostBcAcceptedClientBuilds[i].Build; ++i)
		if (PostBcAcceptedClientBuilds[i].Build == build)
			return &PostBcAcceptedClientBuilds[i];

	for (int i = 0; PreBcAcceptedClientBuilds[i].Build; ++i)
		if (PreBcAcceptedClientBuilds[i].Build == build)
			return &PreBcAcceptedClientBuilds[i];

	return nullptr;
}

}
