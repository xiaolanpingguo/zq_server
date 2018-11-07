#pragma once

#include "IModule.h"

namespace zq {


class IDataAgentModule : public IModule
{
public:

	template <typename T>
	bool getData(const std::string& user_id, const std::string& key, T& obj)
	{
		std::string data;
		if (getRedisData(user_id, key, data))
		{
			return obj.ParseFromString(data);
		}

		return false;
	}

	virtual bool getRedisData(const std::string& user_id, const std::string& key, std::string& data) = 0;
};

}