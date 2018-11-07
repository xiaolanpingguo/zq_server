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
	elementModule_ = libManager_->findModule<IElementModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	redisModule_ = libManager_->findModule<IRedisModule>();

	return true;
}

bool LoginCSModule::initEnd()
{	
	std::shared_ptr<IClass> logic_class = classModule_->getElement(config::Server::this_name());
	if (logic_class)
	{
		const std::vector<std::string>& strIdList = logic_class->getIDList();
		for (size_t i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

			int server_type = elementModule_->getPropertyInt(strId, config::Server::server_type());
			int server_id = elementModule_->getPropertyInt(strId, config::Server::server_id());
			if (server_type == SERVER_TYPES::ST_LOGIN_SERVER && libManager_->getServerID() == server_id)
			{
				//int max_conn = elementModule_->getPropertyInt(strId, config::Server::max_connect());
				int ext_port = elementModule_->getPropertyInt(strId, config::Server::external_port());
				const std::string& ext_ip = elementModule_->getPropertyString(strId, config::Server::external_ip());

				if (ext_port != -1)
				{
					if (!startNetwork(ext_ip, (uint16)ext_port))
					{
						ASSERT(false, "Cannot init external server net");
						return false;
					}
					
				}

				std::ostringstream strLog;
				strLog << std::endl;
				strLog << "-----------login_server is listen on external net-----------" << std::endl;
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

	return true;
}

bool LoginCSModule::createAccount(const std::string& account_name, const std::string& pwd)
{
	std::string account = account_name;
	std::string password = pwd;
	util::utf8ToUpperOnlyLatin(account);
	util::utf8ToUpperOnlyLatin(password);

	std::string key = KEY_ACCOUNT + account;

	RedisClientPtr redis_client = redisModule_->getClientByHash(account);
	if (redis_client == nullptr)
	{
		return false;
	}

	if (redis_client->EXISTS(key))
	{
		return false;
	}

	DBAccount::DBUserAccount account_info;
	account_info.set_is_banned(false);
	account_info.set_account_name(account);
	account_info.set_password(password);
	account_info.set_password_hash(calculateShaPassHash(account, password));

	std::string data;
	account_info.SerializeToString(&data);
	redis_client->SET(key, data);

	return true;
}

}

