#pragma once

#include "IModule.h"
#include <vector>

namespace zq {


class GameSSSession;
using GameSSSessionPtr = std::shared_ptr<GameSSSession>;
using GameSSSessionList = std::vector<GameSSSessionPtr>;
class IGameSSModule : public IModule
{
public:

	virtual GameSSSessionPtr getSessionByServerId(int server_id) = 0;
	virtual void getSessionByServerType(int server_type, GameSSSessionList& vec) = 0;
};

}