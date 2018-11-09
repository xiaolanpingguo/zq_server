#pragma once

#include "interface_header/IDataAgentModule.h"
#include "interface_header/IKernelModule.h"
#include "interface_header/IRedislModule.h"

namespace zq{


class DataAgentModule : public IDataAgentModule
{
public:
	DataAgentModule(ILibManager* p)
	{
		libManager_ = p;
	}

	~DataAgentModule() = default;

	bool init() override;
	bool run() override;

public:

	bool setRedisHashData(const std::string& key, const std::string& field_key, const std::string& data) override;
	bool getRedisHashData(const std::string& key, const std::string& field_key, std::string& data) override;
	bool hexists(const std::string& user_id, const std::string& field_key) override;

private:

	IKernelModule* kernelModule_;
	IRedisModule* redisModule_;
};

}
