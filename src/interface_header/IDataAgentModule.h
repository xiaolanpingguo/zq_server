#pragma once

#include "IModule.h"

namespace zq {


static constexpr const char* _KEY_USER_ = "_user@";  // 玩家的hash key

static constexpr const char* _KEY_ACCOUNT_ = "account@";  // 帐号信息key


class IDataAgentModule : public IModule
{
public:

	template <typename T>
	bool getHashData(const std::string& user_id, const std::string& field_key, T& obj)
	{
		std::string key = _KEY_USER_ + user_id;
		std::string data;
		if (getRedisHashData(key, field_key, data))
		{
			return obj.ParseFromString(data);
		}

		return false;
	}

	template <typename T>
	bool setHashData(const std::string& user_id, const std::string& field_key, T& obj)
	{
		std::string key = _KEY_USER_ + user_id;
		std::string data;
		obj.SerializeToString(&data);
		return setRedisHashData(key, field_key, data);
	}

	virtual bool setRedisHashData(const std::string& key, const std::string& field_key, const std::string& data) = 0;
	virtual bool getRedisHashData(const std::string& key, const std::string& field_key, std::string& data) = 0;
	virtual bool hexists(const std::string& user_id, const std::string& field_key) = 0;
};

}