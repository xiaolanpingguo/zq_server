#pragma once

#include "IModule.h"

namespace zq {




static constexpr const char* _KEY_ACCOUNT_ = "account@";  // 帐号信息key
static constexpr const char* _KEY_PLAYER_ = "player@";  // 玩家角色列表key


class IDataAgentModule : public IModule
{
public:

	template <typename T>
	bool getHashData(const std::string& hash_key, const std::string& field_key, T& obj)
	{
		std::string data;
		if (getRedisHashData(hash_key, field_key, data))
		{
			return obj.ParseFromString(data);
		}

		return false;
	}

	template <typename T>
	bool setHashData(const std::string& hash_key, const std::string& field_key, T& obj)
	{
		std::string data;
		obj.SerializeToString(&data);
		return setRedisHashData(hash_key, field_key, data);
	}

	virtual bool hexists(const std::string& hash_key, const std::string& field_key) = 0;

protected:

	virtual bool setRedisHashData(const std::string& hash_key, const std::string& field_key, const std::string& data) = 0;
	virtual bool getRedisHashData(const std::string& hash_key, const std::string& field_key, std::string& data) = 0;
};

}