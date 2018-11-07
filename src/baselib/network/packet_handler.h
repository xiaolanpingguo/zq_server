//#pragma once
//
//
//#include "channel.h"
//#include "message_buffer.hpp"
//
//namespace zq {
//
//
//// 包的前4个字节储存长度
//constexpr static int HEADER_LENTH = 4;
//
//
//// 默认的数据格式解析
//using FUN_DATA_CB = std::function<void(IChannel* channel, const char* data, const size_t len, int msgid)>;
//class PacketHandler
//{
//public:
//
//	PacketHandler(FUN_DATA_CB&& cb);
//	~PacketHandler();
//
//	virtual int enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid);
//
//	virtual void readHandler(IChannel* channel, MessageBuffer& packet);
//
//protected:
//
//	virtual bool readHeaderLengthHandler();
//	virtual bool readHeaderHandler();
//	virtual bool readDataHandler();
//
//protected:
//
//	IChannel* channel_;
//	MessageBuffer headerLengthBuffer_;
//	MessageBuffer headerBuffer_;
//	MessageBuffer packetBuffer_;
//	FUN_DATA_CB dataCb_;
//};
//
//
//
//
//
///*
//	内部组件通信用的数据格式的解析
//*/
//class ComponentPacketHandler : public PacketHandler
//{
//public:
//
//	ComponentPacketHandler(FUN_DATA_CB&& cb);
//	~ComponentPacketHandler();
//
//	int enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid) override;
//
//protected:
//
//	virtual bool readHeaderLengthHandler() override;
//	virtual bool readHeaderHandler() override;
//	virtual bool readDataHandler() override;
//};
//
//
//
//
//
//
///*
//	websocket数据格式的解析
//*/
//enum FrameType
//{
//	// 下一帧与结束
//	NEXT_FRAME = 0x0,
//	END_FRAME = 0x80,
//
//	ERROR_FRAME = 0xFF00,
//	INCOMPLETE_FRAME = 0xFE00,
//
//	OPENING_FRAME = 0x3300,
//	CLOSING_FRAME = 0x3400,
//
//	// 未完成的帧
//	INCOMPLETE_TEXT_FRAME = 0x01,
//	INCOMPLETE_BINARY_FRAME = 0x02,
//
//	// 文本帧与二进制帧
//	TEXT_FRAME = 0x81,
//	BINARY_FRAME = 0x82,
//
//	PING_FRAME = 0x19,
//	PONG_FRAME = 0x1A,
//
//	// 关闭连接
//	CLOSE_FRAME = 0x08,
//};
//
//enum EnWebSocketHeadLen
//{
//	EN_SHORT_HEADER = 6,
//	EN_MEDIUM_HEADER = 8,
//	EN_LONG_HEADER = 14,
//	INVALID_HEADER
//};
//
//struct WebSocketHead
//{
//	bool has_fin;
//	bool is_masked;
//	short op_code;
//	uint64 payload_size;
//	char mask[4];
//	WebSocketHead()
//	{
//		memset(this, 0, sizeof(WebSocketHead));
//	}
//};
//
//class WebSocketPacketHandler : public PacketHandler
//{
//public:
//
//	WebSocketPacketHandler(FUN_DATA_CB&& cb);
//	~WebSocketPacketHandler();
//
//	void reset();
//
//	int enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid);
//
//	void wsPing();
//	void wsPong();
//
//protected:
//
//	bool parseHead(const char* data_buffer, uint32 buffer_len, uint32& left_size);
//	FrameType getFrameType();
//	void decodeMaskData(char* pszData, int iDataLen, char* pMask);
//
//	virtual bool readHeaderLengthHandler() override;
//	virtual bool readHeaderHandler() override;
//	virtual bool readDataHandler() override;
//
//private:
//
//	int getBit(char byte, int position);
//	void setBit(char * buffer, int position);
//	int get16Bit(const char * buffer);
//	void set16Bit(int value, char * buffer);
//
//private:
//
//	static const int WS_HEAD_SIZE = 2;
//	static const int WS_MASK_SIZE = sizeof(uint32);
//	static const int WS_PAYLOAD_SIZE_UINT16 = sizeof(uint16);
//	static const int WS_PAYLOAD_SIZE_UINT64 = sizeof(uint64);
//
//	WebSocketHead webSocketHead_;
//};
//
//}
//
//
