#include "login_cs_module.h"

#include "baselib/base_code/util.h"
#include "baselib/message/game_db_account.pb.h"
#include "config_header/cfg_server.hpp"

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
	classModule_ = libManager_->findModule<IClassModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	return true;
}

bool LoginCSModule::initEnd()
{
	const auto& all_row = configModule_->getCsvRowAll<CSVServer>();
	for (const auto& ele : *all_row)
	{
		int server_type = ele.second.server_type;
		int server_id = ele.second.server_id;
		if (server_type == SERVER_TYPES::ST_LOGIN_SERVER && libManager_->getServerID() == server_id)
		{
			//int max_conn = ele.second->max_conn;
			int ext_port = ele.second.external_port;
			const std::string& ext_ip = ele.second.external_ip;

			if (ext_port != -1)
			{
				if (!startNetwork(ext_ip, (uint16)ext_port))
				{
					ASSERT(false, "Cannot init login external server net");
					return false;
				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------login_cs_server is listen on external net-----------" << std::endl;
				strLog << "listen ext_address: " << ext_ip + ":" + std::to_string(ext_port) << std::endl;
				std::cout << strLog.str() << std::endl;
				LOG_INFO << strLog.str();
			}
		}
	}

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

	std::string hash_key = _KEY_ACCOUNT_ + account;
	if (dataAgentModule_->hexists(hash_key, account))
	{
		return false;
	}

	DBAccount::DBUserAccount account_info;
	account_info.set_is_banned(false);
	account_info.set_account_name(account);
	account_info.set_password(password);
	account_info.set_password_hash(calculateShaPassHash(account, password));
	dataAgentModule_->setHashData(hash_key, account, account_info);

	return true;
}

}

