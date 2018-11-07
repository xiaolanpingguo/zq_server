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
	ÿ��server�Ŀͻ��˲���(����ȥ������server)�������Ұ��ⲿ�ֳ����һ�������
	��������������ر�˵��һ�¶����������֣������Ϊ���������
	1. ����Ҫȥ���ӵ����������game->world�����game�ҵ��ˣ�world����Ӧ�ѹҵ������game�������ɾ��
	2. �������ӵ���������籾�����world���������game�����ӣ���ʱ���world�ҵ���game�����������б��а�
		world����ɾ��������һֱҪȥ����world��ֱ�������ɹ�
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

	// ����������״̬
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
	std::vector<int> needCheckTypes_;  //�������Ҫȥ���ӵ����

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

