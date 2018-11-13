#pragma once 

#include "interface_header/base/IClassModule.h"
#include "interface_header/base/ITimerModule.h"
#include "interface_header/base/IServerComm.h"
#include "interface_header/base/IGameToWorldModule.h"

#include "game_to_world_session.h"


namespace zq {


class GameToWorldModule : public IGameToWorldModule
{
	using GameToWorldSessionPtr = std::shared_ptr<GameToWorldSession>;
	using WorldConnMapT = std::map<int, GameToWorldSessionPtr>;
public:

	GameToWorldModule(ILibManager* p);

	bool init() override;
	bool initEnd() override;
	bool run() override;

protected:

	void updataSocket();
	void processConn();

	void addConnInfo(ConnServerInfo& info);

	void timerCheckConnect(void* user_data);
	void timerSendHeartBeat(void* user_data);

	void sendHeartBeat(GameToWorldSessionPtr session);

private:

	IClassModule* classModule_;
	ITimerModule* timerModule_;

	WorldConnMapT connsMap_;
	std::vector<ConnServerInfo> connInfo_;
};

}
