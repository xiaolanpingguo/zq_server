#pragma once

#include "IModule.h"
#include <vector>

namespace zq {


class WorldSSSession;
using WorldSSSessionPtr = std::shared_ptr<WorldSSSession>;
using WorldSSSessionList = std::vector<WorldSSSessionPtr>;
class IWorldSSModule : public IModule
{
public:

	virtual WorldSSSessionPtr getSessionByServerId(int server_id) = 0;
	virtual void getSessionByServerType(int server_type, WorldSSSessionList& vec) = 0;
};

}