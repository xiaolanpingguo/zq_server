#pragma once


#include "interface_header/logic/IGameLogicModule.h"
#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/base/IDataAgentModule.h"
#include "interface_header/logic/IAddonsModule.h"
#include <unordered_map>


namespace zq{


class WorldSession;
class WorldPacket;
class AuthCrypt;
class GameLogicModule : public IGameLogicModule
{
	using MsgFunMap = std::unordered_map<int32, MsgCallBackFun>;
public:

	GameLogicModule(ILibManager* p);
	~GameLogicModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

    bool loadFromDB() override;
	bool saveToDB() override;

public:

	bool handleAuthSession(WorldSession* session, WorldPacket& recvPacket);
	bool handlePing(WorldSession* session, WorldPacket& recvPacket);
	bool handleKeepLive(WorldSession* session, WorldPacket& recvPacket);
	void handleSendAuthSession();

	void readAddonsInfo(WorldSession* session, ByteBuffer& data);

	void sendAuthResponseError(WorldSession* session, uint8 code);

	void onWorldSessionInit(WorldSession* session);

protected:

	bool addMsgFun(int32 msgid, MsgCallBackFun&& fun) override;
	void call(int32 msgid, WorldSession* session, WorldPacket& recvPacket) override;

private:

	MsgFunMap msgFuns_;

	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	IRedisModule* redisModule_;
	IDataAgentModule* dataAgentModule_;
	IAddonsModule* addonsModule_;
};

}
