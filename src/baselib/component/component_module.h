#pragma once

#include "baselib/interface_header/IComponentModule.h"
#include "baselib/interface_header/IKernelModule.h"
#include "baselib/interface_header/INetSSModule.h"
#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/ILogModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/IHttpClientModule.h"
#include "baselib/network/connector.hpp"

#include "baselib/message/game_ss.pb.h"

namespace zq {

/*
	每个server的客户端部分(主动去连其他server)，这里我把这部分抽象成一个组件，
	采用组件来管理，特别说明一下断线重连部分，这里分为两种情况，
	1. 主动要去连接的组件，比如game->world，如果game挂掉了，world会相应把挂掉的这个game组件进行删除
	2. 被动连接的组件，比如本组件是world，他会接受game的连接，此时如果world挂掉，game并不会从组件列表中把
		world进行删除，而是一直要去重连world，直到重连成功
*/
class ComponentModule : public IComponentModule
{
	enum EN_STATE
	{
		EN_REGIST_TO_MASTER,
		EN_REQUEST_COMPONETN,
		EN_CONNECT_COMPONENT,
		EN_FIND_ALL_COMPONENT,
	};
public:
	ComponentModule(ILibManager* p);
	~ComponentModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

protected:

	// 检测组件连接状态
	void checkConnState();

	void registerToComonpent(int server_type);
	void serverReport();
	void sendHeartBeatToMaster();

public:

	COMPONENTS& getComponents(int server_type);
	ComponentInfosPtr findComponent(int server_type, int server_id) override;
	ComponentInfosPtr findComponent(int server_id);

	bool addComponent(int server_type, int server_id, const std::string& intaddr, uint16 intport, const std::string& extaddr, uint16 extport,
		int state, int pid = 0, InternalSocketPtr channel = nullptr);

private:

	int state_;
	std::vector<int> needCheckTypes_;  //本组件需要去连接的组件

	COMPONENTS masterInfo_;
	COMPONENTS worldInfo_;
	COMPONENTS loginInfo_;
	COMPONENTS gameInfo_;
	COMPONENTS gateInfo_;
	COMPONENTS controlInfo_;

	std::string masterAddr_;

	IElementModule* elementModule_;
	IKernelModule* kernelModule_;
	INetSSModule* netSSModule_;
	IClassModule* classModule_;
	ILogModule* logModule_;
	IHttpClientModule* httpClientModule_;

	Asio::IoContext ioService_;
};

}

