#pragma once

#include "IModule.h"
#include "baselib/network/INet.h"
#include "baselib/network/internal_socket.hpp"
#include "baselib/core/map_ex.hpp"

namespace zq {


struct ComponentInfos
{
	enum enState
	{
		EN_INIT,
		EN_DISCONNECT,
		EN_CONNECTING,
		EN_HAS_CONNECTED,
		EN_RUN,
	};
	int server_type = 0;
	int server_id = 0;
	std::string ext_ip;
	std::string int_ip;
	uint16 ext_port;
	uint16 int_port;
	int pid = 0;
	InternalSocketPtr channel_ = nullptr;
	int state_ = EN_INIT;
	time_t lastConnectTime_ = 0;
	time_t lastCheckTime_ = 0;
};


using ComponentInfosPtr = std::shared_ptr<ComponentInfos>;
// serverid--ComponentInfos
using COMPONENTS = ConsistentHashMap<int, ComponentInfos>;

class IComponentModule : public IModule
{
public:

	virtual COMPONENTS& getComponents(int server_type) = 0;
	virtual ComponentInfosPtr findComponent(int server_type, int server_id) = 0;
	virtual bool addComponent(int server_type, int server_id, const std::string& intaddr, uint16 intport, const std::string& extaddr, uint16 extport,
		int state, int pid = 0, InternalSocketPtr channel = nullptr) = 0;
};

}
