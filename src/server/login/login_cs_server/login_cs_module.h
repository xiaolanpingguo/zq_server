#pragma once

#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/ILoginCSModule.h"

#include "baselib/network/socket_mgr.hpp"
#include "login_cs_session.h"

namespace zq {

class LoginCSModule : public ILoginCSModule, public SocketMgr<LoginCSSession>
{
	using BaseSockMgr = SocketMgr<LoginCSSession>;
public:
	LoginCSModule(ILibManager* p)
	{
		libManager_ = p;
	}

	bool init() override;
	bool initEnd() override;
	bool run() override;

	const std::string& getIp() override { return BaseSockMgr::getIp(); };
	uint16 getPort() override { return BaseSockMgr::getPort(); };

private:

	IElementModule * elementModule_;
	IClassModule* classModule_;
};

}

