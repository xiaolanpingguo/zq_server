#pragma once


#include "../base/IModule.h"

namespace zq {


class WorldSession;
class WorldPacket;
using MsgCallBackFun = std::function<bool(WorldSession*, WorldPacket&)>;
class IGameLogicModule : public IModule
{
public:

	virtual bool addMsgFun(int32 msgid, MsgCallBackFun&& fun) = 0;
	virtual void call(int32 msgid, WorldSession* session, WorldPacket& recvPacket) = 0;
};

}
