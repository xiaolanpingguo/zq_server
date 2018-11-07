#pragma once 

#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/ITimerModule.h"
#include "baselib/interface_header/IServerComm.h"
#include "baselib/interface_header/IGameToWorldModule.h"

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

	IElementModule* elementModule_;
	IClassModule* classModule_;
	ITimerModule* timerModule_;

	WorldConnMapT connsMap_;
	std::vector<ConnServerInfo> connInfo_;
};

}
