#pragma once


#include "player.h"
#include "interface_header/logic/IPlayerMgrModule.h"
#include "interface_header/logic/IObjectMgrModule.h"

namespace zq {


class PlayerMgrModule : public IPlayerMgrModule
{
public:
	PlayerMgrModule(ILibManager* p);
	virtual ~PlayerMgrModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;


protected:


private:

	IObjectMgrModule* objectMgrModule_;
};

}
