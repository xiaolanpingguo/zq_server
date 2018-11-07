#pragma once 

#include "interface_header/IClassModule.h"
#include "interface_header/IElementModule.h"
#include "interface_header/ITimerModule.h"
#include "interface_header/IServerComm.h"
#include "interface_header/ILoginToWorldModule.h"

#include "login_to_world_session.h"


namespace zq {


class LoginToWorldModule : public ILoginToWorldModule
{
	using LoginToWorldSessionPtr = std::shared_ptr<LoginToWorldSession>;
	using WorldConnMapT = std::map<int, LoginToWorldSessionPtr>;
public:

	LoginToWorldModule(ILibManager* p);

	bool init() override;
	bool initEnd() override;
	bool run() override;

protected:

	void updataSocket();
	void processConn();

	void addConnInfo(ConnServerInfo& info);

	void timerCheckConnect(void* user_data);
	void timerSendHeartBeat(void* user_data);

	void sendHeartBeat(LoginToWorldSessionPtr session);

private:

	IElementModule* elementModule_;
	IClassModule* classModule_;
	ITimerModule* timerModule_;

	WorldConnMapT connsMap_;
	std::vector<ConnServerInfo> connInfo_;
};

}
