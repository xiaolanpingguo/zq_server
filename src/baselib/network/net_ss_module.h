#pragma once

#include "internal_socket_mgr.hpp"
#include "interface_header/INetSSModule.h"
#include "interface_header/ILogModule.h"


namespace zq {


class NetSSModule : public INetSSModule, public SocketMgr<InternalSocket>
{
	using BaseSocketMgr = SocketMgr<InternalSocket>;
	using InternalSocketMapT = std::map<tcp::endpoint, InternalSocketPtr>;
public:

	NetSSModule(ILibManager* p);
	virtual ~NetSSModule();

	bool init() override;

	bool run() override;

	void connect(const std::string& ip, uint16 port, void* user_data, bool is_async = true) override;

	int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) override;

	const std::string& getIp() override { return ip_; }
	uint16 getPort() override { return port_; }

	void setSSDataCb(IntSocketMessgeCbT&& cb) override { messageCb_ = std::move(cb); }
	void setSSEventCb(IntNetEventCbT&& cb) override { eventCb_ = std::move(cb); }

protected:

	void onAccept(tcp::socket&& sock, uint32 threadIndex);
	InternalSocketPtr newSocket(tcp::socket&& sock);

	void checkSocket();
	bool removeSocket(InternalSocketPtr socket);

	void onNetEvent(InternalSocketPtr channel, bool conn, void* user_data);
	void onChannelDataCb(IChannel* channel, const void* data, size_t len);
	void onChannelTimeout(IChannel* channel);

	bool addChannel(InternalSocketPtr channel);

private:

	std::string ip_;
	uint16 port_;

	// 系统发送缓冲区
	int32 socketSystemSendBufferSize_;

	// 用户的消息回调
	IntSocketMessgeCbT messageCb_;

	// 连接的事件回调
	IntNetEventCbT eventCb_;

	// 保存所有的socket
	InternalSocketMapT intSocketMap_;

	// connector
	ConnectorPtr connector_;

	ILogModule* logModule_;
};

}
