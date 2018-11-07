//#pragma once
//
//
//#include <functional>
//#include <memory>
//#include <vector>
//#include "message_buffer.hpp"
//#include "byte_buffer.hpp"
//#include "baselib/interface_header/platform.h"
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#include <WS2tcpip.h>
//#else
//#include <sys/socket.h>	 //AF_INET
//#include <arpa/inet.h> // htonl
//#endif
//
//
//namespace zq {
//
//
//#define BUFFER_MAX_READ		65535
//#define BUFFER_MAX_WRITE	65535
//
//enum EN_NET_EVENT
//{
//	EN_NET_EVENT_EOF = 0x10,
//	EN_NET_EVENT_ERROR = 0x20,
//	EN_NET_EVENT_TIMEOUT = 0x40,
//	EN_NET_EVENT_CONNECTED = 0x80,
//};
//
//
//class INet;
//class IChannel;
//
//using NET_READ_DATA_FUN = std::function<void(IChannel* channel, MessageBuffer& buffer)>;
//using NET_RECEIVE_FUN = std::function<bool(IChannel* channel, const void* data, size_t len, int msgid)>;
//using NET_EVENT_FUN = std::function<void(IChannel* channel, const EN_NET_EVENT eEvent)>;
//
//// һ��ͨ��Ҫ���Ƴ�ʱ�����Ȼص��������
//using CHANNEL_REMOVE_CB = std::function<void(IChannel* channel)>;
//
//// һ��ͨ����ʱʱ�����Ȼص��������
//using CHANNEL_TIMEOUT_CB = std::function<void(IChannel* channel)>;
//
//
////class Address
////{
////public:
////
////	inline Address(uint32 ip = 0, uint16 port = 0) :ip_(ip), port_(port) {}
////	inline Address(const std::string& ip, uint16 port)
////		: port_(::htons(port))
////	{
////		Address::string2ip(ip.c_str(), ip_);
////	}
////	inline Address(const Address& that)
////		: ip_(that.ip_), port_(that.port_)
////	{
////	}
////	inline Address& operator=(const Address& that)
////	{
////		if (this != &that)
////		{
////			this->ip_ = that.ip_;
////			this->port_ = that.port_;
////		}
////
////		return *this;
////	}
////	inline virtual ~Address() {}
////
////	bool isNone() const { return this->ip_ == 0; }
////
////	const char* c_str() const
////	{
////		static std::string s_stringBuf[2];
////		static int cur_index = 0;
////		cur_index = (cur_index + 1) % 2;
////		std::string& cur_str = s_stringBuf[cur_index];
////		std::string str_ip;
////		ip2string(ip_, str_ip);
////		cur_str = str_ip + ":" + std::to_string(::ntohs(port_));
////		return cur_str.c_str();
////	}
////
////	static int string2ip(const char* string, uint32& address, int protocal = AF_INET/*AF_INET6*/)
////	{
////		// �ɹ�����1, �����벻����Ч�ĸ�ʽ,����0,����ʧ��, ����-1
////		struct ::in_addr sa;; // IPv4��ַ�ṹ��
////		int ret = inet_pton(protocal, string, &sa);
////		if (ret)
////		{
////			address = sa.s_addr;
////		}
////
////		return ret;
////	}
////
////	static int ip2string(uint32 address, std::string& str, int protocal = AF_INET/*AF_INET6*/)
////	{
////		// �ɹ�,����ָ������ָ��, ���򷵻�NULL 
////		static char ip_buf[128] = { 0 };
////		memset(ip_buf, 0, sizeof(ip_buf));	  //AF_INET6
////		struct ::in_addr sa;
////		sa.s_addr = address;
////		inet_ntop(AF_INET, (const void*)&sa, ip_buf, sizeof(ip_buf) - 1);  //AF_INET6
////		str = ip_buf;
////
////		return 0;
////	}
////
////	uint32 ip_;
////	uint16 port_;
////};
////
////// �Ƚϲ���������
////inline bool operator==(const Address& a, const Address& b)
////{
////	return (a.ip_ == b.ip_) && (a.port_ == b.port_);
////}
////
////inline bool operator!=(const Address& a, const Address& b)
////{
////	return (a.ip_ != b.ip_) || (a.port_ != b.port_);
////}
////
////inline bool operator<(const Address& a, const Address& b)
////{
////	return (a.ip_ < b.ip_) || (a.ip_ == b.ip_ && (a.port_ < b.port_));
////}
////
////
////class IChannel
////{
////public:
////
////	enum EnFlags
////	{
////		FLAG_HANDSHAKE = 0x00000001,// �Ѿ����ֹ�
////		FLAG_RUNNING = 0x00000002,	// ��������
////		FLAG_CONDEMN = 0x00000004,	// ��ͨ���Ѿ���÷Ƿ���
////		FLAG_TIMEOUT = 0x00000008,	// ��ͨ���Ѿ���ʱ��
////	};
////
////	virtual void close() = 0;
////	virtual bool sendData(const void* data, const size_t len, int msgid = 0, bool encode = true) = 0;
////	virtual	INet* getNet() = 0;
////	virtual const char* c_str() = 0;
////	virtual const Address& getAddr() = 0;
////	virtual bool isInternal() = 0;
////	virtual void updateTick() = 0;
////
////	void setFlags(uint32 flag, bool add)
////	{
////		if (add)
////			flags_ |= flag;
////		else
////			flags_ &= ~flag;
////	}
////	bool checkFlags(uint32 flag)
////	{
////		return (flags_ & flag) > 0;
////	}
////
////protected:
////	uint32 flags_ = 0;
////};
//
//class INet
//{
//public:
//	virtual ~INet()
//	{}
//
//	virtual bool run() = 0;
//
//	// ��ʼ��server��client
//	virtual IChannel* createClientChannel(const Address& addr, bool is_internal) = 0;
//	virtual int initServerNet(const std::string& ip, unsigned short port, bool is_internal,
//		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;
//
//	// ����
//	virtual bool finalize() = 0;
//
//	virtual bool sendDataToAllClient(const char* data, const size_t len, int msgid = 0) = 0;
//
//	virtual IChannel* getChannel(const Address& addr) = 0;
//
//	virtual bool log(int severity, const char* msg) = 0;
//
//	// ���ݻص�
//	virtual void setReadDataCb(NET_READ_DATA_FUN&& cb) {};
//
//	// ����ͨ���Ƴ��ͳ�ʱ�Ļص�
//	virtual void setChannelDeregisterCb(CHANNEL_REMOVE_CB&& cb) {};
//	virtual void setChannelTimeOutCb(CHANNEL_TIMEOUT_CB&& cb) {};
//
//	// ����ⲿ��ַ���ڲ���ַ
//	virtual const Address& getIntAddr() = 0;
//	virtual const Address& getExtAddr() = 0;
//};
//
//
//}
//
