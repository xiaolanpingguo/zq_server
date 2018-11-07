#pragma once

#include "lib/interface_header/IClassModule.h"
#include "lib/interface_header/ILogModule.h"
#include "lib/interface_header/IElementModule.h"
#include "lib/interface_header/IRedislModule.h"

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

	IElementModule * elementModule_;
	IClassModule* classModule_;
	ILogModule* logModule_;

	RedisServerMapT serverMap_;
};

}
