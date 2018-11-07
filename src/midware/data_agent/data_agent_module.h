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

	bool getRedisData(const std::string& user_id, const std::string& key, std::string& data);

private:

	IKernelModule* kernelModule_;
	IRedisModule* redisModule_;
};

}
