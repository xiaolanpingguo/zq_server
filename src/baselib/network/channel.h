//#pragma once
//
//
//#include "INet.h"
//#include "message_buffer.hpp"
//
//namespace zq {
//
//
//class PacketHandler;
//class CNet;
//class Channel : public IChannel
//{
//	enum EnChannelType
//	{
//		CHANNEL_NORMALSOCKET = 0,
//		CHANNEL_WEBSOCKET = 1,
//	};
//
//public:
//	Channel(CNet* pNet, const Address& addr, void* pBev, bool is_internal);
//	virtual ~Channel();
//
//	size_t addBuff(const char* str, size_t nLen);
//
//	int enCode(const void* strData, size_t unDataLen, std::string& strOutData, int msgid);
//	int deCode();
//
//	INet* getNet() override;
//	void close() override;
//	const char* c_str() override;
//	const Address& getAddr() { return addr_; }
//	bool isInternal() override { return internal_; }
//	bool sendData(const void* data, const size_t len, int msgid = 0, bool encode = true) override;
//	void updateTick() override;
//
//	void setUserData(void* data) { userData_ = data; }
//	void* getUserData() { return userData_; }
//
//	bool handshake();
//
//	void recvCb(const char* data, size_t len, int msgid = 0);
//
//	MessageBuffer& getReadBuffer() { return readBuffer_; }
//
//	void setTimeout(int second) { channelTimeout_ = second; }
//
//private:
//
//	bool isWebSocketProtocol(const std::string& str);
//	bool handShakeWebSocket(const std::string& str);
//
//private:
//
//	Address addr_;
//
//	uint64 sendPakcetNum_;
//	uint64 recvPakcetNum_;
//	EnChannelType channelType_;
//
//	void* userData_;
//	bool internal_;
//	bool needRemove_;
//
//	time_t lastActTime_;
//	int channelTimeout_;
//
//	MessageBuffer readBuffer_;
//
//	std::shared_ptr<PacketHandler> packetHandler_;
//
//	// 这里不用INet的原因在于，channel可能会去访问CNet没有暴露的接口
//	CNet* inet_;
//};
//
//}