//#pragma once
//
//#include "INet.h"
//#include "channel.h"
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
//
//#include <netinet/in.h>
//#ifdef _XOPEN_SOURCE_EXTENDED
//#  include <arpa/inet.h>
//# endif
//#include <sys/socket.h>
//
//#endif
//
//#include <map>
//#include <vector>
//#include <limits>
//
//#include <event2/bufferevent.h>
//#include <event2/buffer.h>
//#include <event2/listener.h>
//#include <event2/util.h>
//#include <event2/thread.h>
//#include <event2/event_compat.h>
//#include <event2/bufferevent_struct.h>
//#include <event2/event.h>
//
//namespace zq {
//
//
//class CNet : public INet
//{
//public:
//	CNet();
//	CNet(NET_RECEIVE_FUN&& recv_cb, NET_EVENT_FUN&& event_cb);
//	virtual ~CNet();
//
//public:
//	virtual bool run();
//
//	IChannel* createClientChannel(const Address& addr, bool is_internal = true) override;
//	int initServerNet(const std::string& ip, unsigned short port, bool is_internal,
//		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) override;
//
//	virtual bool finalize();
//
//	virtual bool sendDataToAllClient(const char* data, const size_t len, int msgid = 0) override;
//
//	virtual bool closeChannel(Channel* channel);
//	virtual bool closeChannel(const Address& addr);
//	IChannel* getChannel(const Address& addr) override;
//
//	virtual bool log(int severity, const char* data);
//
//	const Address& getIntAddr() override { return intAddr_; }
//	const Address& getExtAddr() override { return extAddr_; }
//
//	void handleMessage(Channel* channel, const char* data, size_t len, int msgid = 0);
//
//	void setReadDataCb(NET_READ_DATA_FUN&& cb) override { readDataCb_ = std::move(cb); }
//
//	// ͨ���Ƴ�ʱ�Ļص�
//	void setChannelDeregisterCb(CHANNEL_REMOVE_CB&& cb) override { channelDeregisterCb_ = std::move(cb); }
//	void setChannelTimeOutCb(CHANNEL_TIMEOUT_CB&& cb) override { channelTimeoutCb_ = std::move(cb); }
//
//private:
//
//	bool addChannel(const Address& addr, Channel* channel);
//
//	bool initEventBase();
//
//	void executeClose();
//	bool closeSocketAll();
//
//	bool processChannels();
//
//	void onChannelTimeout(Channel* channel);
//
//	static void new_conn(evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data, bool internal);
//	static void int_listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data);
//	static void ext_listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data);
//	static void conn_readcb(struct bufferevent* bev, void* user_data);
//	static void conn_writecb(struct bufferevent* bev, void* user_data);
//	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);
//	static void log_cb(int severity, const char* data);
//
//private:
//
//	// ���е�ͨ��
//	std::map<Address, Channel*> channels_;
//
//	// �������Ƴ���ͨ��
//	std::vector<Address> removeChannels_;
//
//	// ���������
//	int maxConn_;
//
//	// ��д��������С
//	int wbufferSize_;
//	int rbufferSize_;
//
//	// ͨ����ʱʱ��(��)
//	int channelTimeout_;
//
//	// һЩͳ��
//	int64 sendMsgTotal_;
//	int64 receiveMsgTotal_;
//
//	// �ⲿ��ַ���ڲ���ַ
//	Address extAddr_;
//	Address intAddr_;
//
//	struct event_base* eventBase_;
//	struct evconnlistener* extListener_;
//	struct evconnlistener* intListener_;
//
//	// ���ݻص�
//	NET_READ_DATA_FUN readDataCb_;
//
//	// ͨ�����Ƴ��Ļص�
//	CHANNEL_REMOVE_CB channelDeregisterCb_;
//
//	// ͨ����ʱ�Ļص�
//	CHANNEL_TIMEOUT_CB channelTimeoutCb_;
//
//	// ��Ϣ�����Ļص�
//	NET_RECEIVE_FUN recvCb_;
//
//	// �¼��ص�
//	NET_EVENT_FUN eventCb_;
//};
//
//
//}
//
