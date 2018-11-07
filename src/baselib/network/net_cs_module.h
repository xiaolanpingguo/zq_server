#pragma once

#include "external_socket.hpp"
#include "interface_header/INetCSModule.h"
#include "interface_header/ILogModule.h"


namespace zq {


class NetCSModule : public INetCSModule, public SocketMgr<ExternalSocket>
{
	using BaseSocketMgr = SocketMgr<ExternalSocket>;
	using ChannelMapT = std::map<tcp::endpoint, IChannelPtr>;

public:

	NetCSModule(ILibManager* p);
	virtual ~NetCSModule();

	bool init() override;

	int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) override;

	bool run() override;

	bool addChannel(IChannelPtr channel);

	void onAccept(tcp::socket&& sock, uint32 threadIndex);

	const std::string& getIp() override { return ip_; }
	uint16 getPort() override { return port_; }

	void setCSDataCb(ExtSocketMessgeCbT&& cb) override { messageCb_ = std::move(cb); }
	void setPacketReadHandler(ExtPacketReaderHandleT&& cb) override { packetReadHanlder_ = std::move(cb); }

protected:

	void onChannelDataCb(IChannel* channel, const void* data, size_t len);
	void onChannelTimeout(IChannel* channel);

private:

	ILogModule* logModule_;
	uint64 lastActTime_;

	int maxConn_;
	int wbufferSize_;
	int rbufferSize_;

	int64 sendMsgTotal_;
	int64 receiveMsgTotal_;

	std::string ip_;
	uint16 port_;

	// 包读取的handler
	ExtPacketReaderHandleT packetReadHanlder_;

	// 系统发送缓冲区
	int32 socketSystemSendBufferSize_;

	// 用户的消息回调
	ExtSocketMessgeCbT messageCb_;

	// 保存所有的socket
	ChannelMapT intSocketMap_;
};

}
