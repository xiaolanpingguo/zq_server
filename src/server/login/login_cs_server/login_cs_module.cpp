#include "login_cs_module.h"

#include "baselib/base_code/util.h"
#include "baselib/message/config_define.hpp"
#include "baselib/message/game_db_account.pb.h"

#include "midware/cryptography/sha1.h"

namespace zq {


static inline std::string calculateShaPassHash(std::string const& name, std::string const& password)
{
	SHA1Hash sha;
	sha.Initialize();
	sha.UpdateData(name);
	sha.UpdateData(":");
	sha.UpdateData(password);
	sha.Finalize();

	return SHA1Hash::byteArrayToHexStr(sha.GetDigest(), sha.GetLength());
}

bool LoginCSModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	redisModule_ = libManager_->findModule<IRedisModule>();
	dataAgentModule_ = libManager_->findModule<IDataAgentModule>();

	return true;
}

bool LoginCSModule::initEnd()
{	

	return true;
}

bool LoginCSModule::run()
{
	update();
	return true;
}

bool LoginCSModule::createAccount(const std::string& account_name, const std::string& pwd)
{
	std::string account = account_name;
	std::string password = pwd;
	util::utf8ToUpperOnlyLatin(account);
	util::utf8ToUpperOnlyLatin(password);

	std::string field_key = _KEY_ACCOUNT_ + account;
	if (dataAgentModule_->hexists(account, field_key))
	{
		return false;
	}

	DBAccount::DBUserAccount account_info;
	account_info.set_is_banned(false);
	account_info.set_account_name(account);
	account_info.set_password(password);
	account_info.set_password_hash(calculateShaPassHash(account, password));
	dataAgentModule_->setHashData(account, field_key, account_info);

	return true;
}

}

