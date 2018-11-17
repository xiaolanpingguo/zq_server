#pragma once

#include "interface_header/base/IClassModule.h"
#include "interface_header/base/ILogModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/base/IConfigModule.h"

namespace zq {


class RedisClient;
class RedisModule : public IRedisModule
{
	using ClientPtr = std::shared_ptr<RedisClient>;
	using RedisServerMapT = ConsistentHashMap<std::string, RedisClient>;

public:

	RedisModule(ILibManager* p);
	virtual ~RedisModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool finalize() override;

	bool addServer(const std::string& id, const std::string& ip, unsigned short port, bool is_syn = true, const std::string& password = "") override;

	RedisClientPtr getClientById(const std::string& strID) override;
	RedisClientPtr getClientBySuitConsistent() override;
	RedisClientPtr getClientByHash(const std::string& strID) override;

protected:

	IClassModule* classModule_;
	ILogModule* logModule_;
	IConfigModule* configModule_;

	RedisServerMapT serverMap_;
};

}
