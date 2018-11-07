#pragma once 


#include "baselib/interface_header/IComponentModule.h"
#include "baselib/network/internal_socket.hpp"


namespace SSMsg
{
	class SSPacket;
}

namespace zq {


class WorldSSSession : public InternalSocket
{
public:

	WorldSSSession(tcp::socket&& socket);
	~WorldSSSession();

	void start() override;
	void onClose() override;
	bool readDataHandler() override;

	int getServerId() { return componentInfos_.server_id; }
	int getServerType() { return componentInfos_.server_type; }
	const ComponentInfos& getCompontent() { return componentInfos_; }

public:
	bool onS2SServerRegisterReq(const SSMsg::SSPacket& packet);
	bool onS2SHeartBeat(const SSMsg::SSPacket& packet);

private:

	ComponentInfos componentInfos_;
};

}
