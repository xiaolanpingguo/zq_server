#pragma once 

#include "midware/cryptography/big_number.h"
#include "baselib/network/socket.hpp"
#include "baselib/network/locked_queue.hpp"
#include "baselib/network/byte_buffer.hpp"

namespace zq {


static constexpr const char* KEY_ACCOUNT = "_account@";

struct AuthHandler;
struct RealmBuildInfo;
class LoginCSSession;
class WorldPacket;

enum AuthStatus
{
	STATUS_CHALLENGE = 0,
	STATUS_LOGON_PROOF,
	STATUS_RECONNECT_PROOF,
	STATUS_AUTHED,
	STATUS_WAITING_FOR_REALM_LIST,
	STATUS_CLOSED
};



enum AuthResult : uint8
{
	WOW_SUCCESS = 0x00,
	WOW_FAIL_BANNED = 0x03,
	WOW_FAIL_UNKNOWN_ACCOUNT = 0x04,
	WOW_FAIL_INCORRECT_PASSWORD = 0x05,
	WOW_FAIL_ALREADY_ONLINE = 0x06,
	WOW_FAIL_NO_TIME = 0x07,
	WOW_FAIL_DB_BUSY = 0x08,
	WOW_FAIL_VERSION_INVALID = 0x09,
	WOW_FAIL_VERSION_UPDATE = 0x0A,
	WOW_FAIL_INVALID_SERVER = 0x0B,
	WOW_FAIL_SUSPENDED = 0x0C,
	WOW_FAIL_FAIL_NOACCESS = 0x0D,
	WOW_SUCCESS_SURVEY = 0x0E,
	WOW_FAIL_PARENTCONTROL = 0x0F,
	WOW_FAIL_LOCKED_ENFORCED = 0x10,
	WOW_FAIL_TRIAL_ENDED = 0x11,
	WOW_FAIL_USE_BATTLENET = 0x12,
	WOW_FAIL_ANTI_INDULGENCE = 0x13,
	WOW_FAIL_EXPIRED = 0x14,
	WOW_FAIL_NO_GAME_ACCOUNT = 0x15,
	WOW_FAIL_CHARGEBACK = 0x16,
	WOW_FAIL_INTERNET_GAME_ROOM_WITHOUT_BNET = 0x17,
	WOW_FAIL_GAME_ACCOUNT_LOCKED = 0x18,
	WOW_FAIL_UNLOCKABLE_LOCK = 0x19,
	WOW_FAIL_CONVERSION_REQUIRED = 0x20,
	WOW_FAIL_DISCONNECTED = 0xFF
};

enum LoginResult
{
	LOGIN_OK = 0x00,
	LOGIN_FAILED = 0x01,
	LOGIN_FAILED2 = 0x02,
	LOGIN_BANNED = 0x03,
	LOGIN_UNKNOWN_ACCOUNT = 0x04,
	LOGIN_UNKNOWN_ACCOUNT3 = 0x05,
	LOGIN_ALREADYONLINE = 0x06,
	LOGIN_NOTIME = 0x07,
	LOGIN_DBBUSY = 0x08,
	LOGIN_BADVERSION = 0x09,
	LOGIN_DOWNLOAD_FILE = 0x0A,
	LOGIN_FAILED3 = 0x0B,
	LOGIN_SUSPENDED = 0x0C,
	LOGIN_FAILED4 = 0x0D,
	LOGIN_CONNECTED = 0x0E,
	LOGIN_PARENTALCONTROL = 0x0F,
	LOGIN_LOCKED_ENFORCED = 0x10
};

enum ExpansionFlags
{
	POST_BC_EXP_FLAG = 0x2,
	PRE_BC_EXP_FLAG = 0x1,
	NO_VALID_EXP_FLAG = 0x0
};

enum RealmFlags : uint8
{
	REALM_FLAG_NONE = 0x00,
	REALM_FLAG_VERSION_MISMATCH = 0x01,
	REALM_FLAG_OFFLINE = 0x02,
	REALM_FLAG_SPECIFYBUILD = 0x04,
	REALM_FLAG_UNK1 = 0x08,
	REALM_FLAG_UNK2 = 0x10,
	REALM_FLAG_RECOMMENDED = 0x20,
	REALM_FLAG_NEW = 0x40,
	REALM_FLAG_FULL = 0x80
};

enum AccountTypes : uint8
{
	SEC_PLAYER = 0,
	SEC_MODERATOR = 1,
	SEC_GAMEMASTER = 2,
	SEC_ADMINISTRATOR = 3,
	SEC_CONSOLE = 4             // must be always last in list, accounts must have less security level always also
};

struct AccountInfo
{
	uint32 Id = 0;
	std::string Login;
	bool IsLockedToIP = false;
	std::string LockCountry;
	std::string LastIP;
	uint32 FailedLogins = 0;
	bool IsBanned = false;
	bool IsPermanenetlyBanned = false;
	AccountTypes SecurityLevel = SEC_PLAYER;
	std::string TokenKey;
};

struct RealmBuildInfo
{
	int Build;
	int MajorVersion;
	int MinorVersion;
	int BugfixVersion;
	int HotfixVersion;
};

struct GameSvrInfo
{
	uint32 build = 0;
	float populationLevel = 0.0f;
	uint16 port = 0;
	uint8 icon = 0;
	uint8 time_zone = 0;
	AccountTypes allowed_security_level = SEC_PLAYER;
	RealmFlags flag = REALM_FLAG_NONE;
	std::string name;
	std::string ip; // eg:127.0.0.1:8085
};

using GameSvrInfoPtr = std::unique_ptr<GameSvrInfo>;
using GameSvrListT = std::vector<GameSvrInfoPtr>;


// WOW登录是采用了SRP算法(安全远程口令)，这里有几个概念需要弄清楚
// N 一个非常大的素数(N=2q+1,q是一个素数，所有的运算都是在mod N 上完成 
// g mod N上的生成元
// k 乘数因子(SRP - 6中，k = 3)
// s 用户的盐值(saltsalt)
// I 用户名
// p 明文口令
// H() 单向hash
// ^ 模幂运算
// u 随机计算值
// a, b 临时秘密值(Secret ephemeral values)
// A, B 临时公开值(Public ephemeral values)
// x 私钥(由p和s生成)
// v 口令校验值
class LoginCSSession : public Socket<LoginCSSession>
{
	typedef Socket<LoginCSSession> BaseSocket;

public:

	LoginCSSession(tcp::socket&& socket);

	void start() override;
	bool update() override;

	void SendPacket(ByteBuffer& packet);

	bool HandleLogonChallenge();
	bool HandleLogonProof();
	bool HandleReconnectChallenge();
	bool HandleReconnectProof();
	bool HandleRealmList();

protected:
	void readHandler() override;

private:

	RealmBuildInfo const* GetBuildInfo(int build);
	bool IsAcceptedClientBuild(int build);
	bool IsPostBCAcceptedClientBuild(int build);
	bool IsPreBCAcceptedClientBuild(int build);

	BigNumber N, s, g, v;
	BigNumber b, B;
	BigNumber K;
	BigNumber _reconnectProof;

	AuthStatus _status;
	AccountInfo _accountInfo;
	std::string _tokenKey;
	std::string _localizationName;
	std::string _os;
	std::string _ipCountry;
	uint16 _build;
	uint8 _expversion;

	LockedQueue<WorldPacket*> _recvQueue;

	GameSvrListT gameSvrList_;
};

}
