//#include "net.h"
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#include <Iphlpapi.h> // GetAdaptersInfo
//#pragma comment (lib,"iphlpapi.lib") // GetAdaptersInfo
//#pragma  comment(lib,"Ws2_32.lib")
//#ifndef LIBEVENT_SRC
//#pragma  comment(lib,"libevent.lib")
//#endif
//#endif
//
//using namespace zq;
///*
//*MODIFY--libevent/buffer.c
//#define EVBUFFER_MAX_READ	4096
//TO
//#define EVBUFFER_MAX_READ	65536
//*/
//
////1048576 = 1024 * 1024
//
//// 要修改4096的限制需要做
//// 1，在accept回调中获取到socket套接字后设置一下socket缓冲区大小（不设置就是8KB）
//// 2，修改libevent源码的EVBUFFER_MAX_READ改大（改好后重新编译libevent）
//// 3，调用bufferevent_set_max_single_read
//
//constexpr static int CHANNEL_TIMEOUT = -1;
//
//void CNet::conn_writecb(struct bufferevent* bev, void* user_data)
//{  
//
//}
//
//void CNet::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
//{
//    Channel* channel = (Channel*)user_data;
//    CNet* pNet = (CNet*)channel->getNet();
//
//    if (events & BEV_EVENT_CONNECTED)
//    {
//		channel->setFlags(IChannel::FLAG_RUNNING, true);
//
//		struct evbuffer* input = bufferevent_get_input(bev);
//		struct evbuffer* output = bufferevent_get_output(bev);
//		if (pNet->wbufferSize_ > 0)
//		{
//			evbuffer_expand(input, pNet->wbufferSize_);
//		}
//		if (pNet->rbufferSize_ > 0)
//		{
//			evbuffer_expand(output, pNet->rbufferSize_);
//		}
//
//		if (pNet->eventCb_)
//		{
//			pNet->eventCb_(channel, EN_NET_EVENT(events));
//		}
//    }
//    else
//    {
//		if (pNet->eventCb_)
//		{
//			pNet->eventCb_(channel, EN_NET_EVENT(events));
//		}
//
//        pNet->closeChannel(channel->getAddr());
//    }
//}
//
//void CNet::new_conn(evutil_socket_t fd, struct ::sockaddr* sa, int socklen, void* user_data, bool is_internal)
//{
//	CNet* pNet = (CNet*)user_data;
//
//	if (pNet->maxConn_ > 0 && (int)pNet->channels_.size() >= pNet->maxConn_)
//	{
//		return;
//	}
//
//	struct event_base* eventBase_ = pNet->eventBase_;
//
//	struct bufferevent* bev = bufferevent_socket_new(eventBase_, fd, BEV_OPT_CLOSE_ON_FREE);
//	if (!bev)
//	{
//		return;
//	}
//
//	struct ::sockaddr_in* pSin = (::sockaddr_in*)sa;
//	Address addr(pSin->sin_addr.s_addr, pSin->sin_port);
//	IChannel* exit_channel = pNet->getChannel(addr);
//	if (exit_channel != nullptr)
//	{
//		LOG_ERROR << "channel is exit,channel: " << exit_channel->c_str();
//		return;
//	}
//
//	Channel* channel = new Channel(pNet, addr, bev, is_internal);
//	pNet->addChannel(addr, channel);
//	channel->setFlags(IChannel::FLAG_RUNNING, true);
//	channel->setTimeout(pNet->channelTimeout_);
//
//	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)channel);
//	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_CLOSED | EV_TIMEOUT);
//	conn_eventcb(bev, BEV_EVENT_CONNECTED, (void*)channel);
//}
//
//void CNet::int_listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
//{
//	new_conn(fd, sa, socklen, user_data, true);
//}
//
//void CNet::ext_listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
//{  
//	new_conn(fd, sa, socklen, user_data, false);
//}
//
//void CNet::conn_readcb(struct bufferevent* bev, void* user_data)
//{
//    Channel* channel = (Channel*)user_data;
//    if (!channel)
//    {
//        return;
//    }
//
//    CNet* pNet = (CNet*)channel->getNet();
//    if (!pNet)
//    {
//        return;
//    }
//
//	// 已经非法了
//    if (channel->checkFlags(IChannel::FLAG_CONDEMN))
//    {
//        return;
//	}
//
//    struct evbuffer* input = bufferevent_get_input(bev);
//    if (!input)
//    {
//        return;
//    }
//
//    size_t len = evbuffer_get_length(input);
//    unsigned char* pData = evbuffer_pullup(input, len);
//    len = channel->addBuff((const char*)pData, len);
//    evbuffer_drain(input, len);
//
//	// 还未连接成功
//	if (!channel->checkFlags(IChannel::FLAG_RUNNING))
//	{
//		return;
//	}
//
//	if (pNet->readDataCb_)
//	{
//		MessageBuffer& buffer = channel->getReadBuffer();
//		pNet->readDataCb_(channel, buffer);
//
//		// 确保下次读取数据有足够的空间
//		buffer.normalize();
//		buffer.ensureFreeSpace();
//	}
//}
//
//CNet::CNet()
//	:
//	eventBase_(nullptr),
//	intListener_(nullptr),
//	extListener_(nullptr),
//	sendMsgTotal_(0),
//	receiveMsgTotal_(0),
//	channelTimeout_(CHANNEL_TIMEOUT),
//	wbufferSize_(BUFFER_MAX_WRITE),
//	rbufferSize_(BUFFER_MAX_READ),
//	readDataCb_(nullptr)
//{
//}
//
//CNet::CNet(NET_RECEIVE_FUN&& recv_cb, NET_EVENT_FUN&& event_cb)
//	:
//	eventBase_(nullptr),
//	intListener_(nullptr),
//	extListener_(nullptr),
//	sendMsgTotal_(0),
//	receiveMsgTotal_(0),
//	channelTimeout_(CHANNEL_TIMEOUT),
//	wbufferSize_(BUFFER_MAX_WRITE),
//	rbufferSize_(BUFFER_MAX_READ),
//	recvCb_(std::move(recv_cb)),
//	eventCb_(std::move(event_cb)),
//	readDataCb_(nullptr)
//{
//}
//
//CNet::~CNet()
//{
//	finalize();
//}
//
//bool CNet::initEventBase()
//{
//	if (eventBase_)
//	{
//		return true;
//	}
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//	WSADATA wsa_data;
//	WSAStartup(0x0201, &wsa_data);
//#endif
//
//	struct event_config* cfg = event_config_new();
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//
//	eventBase_ = event_base_new_with_config(cfg);
//
//#else
//
//	//event_config_avoid_method(cfg, "epoll");
//	if (event_config_set_flag(cfg, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST) < 0)
//	{
//		return false;
//	}
//
//	int cpu_count = 1;
//	if (event_config_set_num_cpus_hint(cfg, cpu_count) < 0)
//	{
//		return false;
//	}
//
//	eventBase_ = event_base_new_with_config(cfg);//event_base_new()
//
//#endif
//	event_config_free(cfg);
//
//	if (!eventBase_)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool CNet::run()
//{
//	executeClose();
//	processChannels();
//
//	if (eventBase_)
//	{
//		event_base_loop(eventBase_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
//	}
//
//	return true;
//}
//
//int CNet::initServerNet(const std::string& ip, unsigned short port, bool is_internal,
//	int rbuffer, int wbuffer, int max_conn)
//{
//	if (eventBase_ == nullptr)
//	{
//		if (!initEventBase())
//		{
//			return -1;
//		}
//	}
//
//	Address addr(ip, port);
//	struct ::evconnlistener* listener = nullptr;
//
//	struct sockaddr_in sin;
//	memset(&sin, 0, sizeof(sin));
//	sin.sin_family = AF_INET;
//	sin.sin_port = addr.port_;
//	sin.sin_addr.s_addr = addr.ip_; //inet_addr("0.0.0.0"); 
//	//sin.sin_addr.s_addr = inet_addr(ip_addr.c_str());
//
//	if (is_internal)
//	{
//		listener = intListener_;
//		listener = evconnlistener_new_bind(eventBase_, int_listener_cb, (void*)this,
//			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
//	}
//	else
//	{
//		listener = extListener_;
//		listener = evconnlistener_new_bind(eventBase_, ext_listener_cb, (void*)this,
//			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
//	}
//
//
//	if (listener == nullptr)
//	{
//		return -2;
//	}
//
//	if (is_internal)
//	{
//		intAddr_ = addr;
//	}
//	else
//	{
//		extAddr_ = addr;
//	}
//
//	event_set_log_callback(&CNet::log_cb);
//
//	return 0;
//}
//
//IChannel* CNet::createClientChannel(const Address& addr, bool is_internal)
//{
//	auto it = channels_.find(addr);
//	if (it != channels_.end())
//	{
//		closeChannel(addr);
//	}
//
//	if (eventBase_ == nullptr)
//	{
//		if (!initEventBase())
//		{
//			return nullptr;
//		}
//	}
//
//	struct sockaddr_in si;
//	memset(&si, 0, sizeof(si));
//	si.sin_family = AF_INET;
//	si.sin_port = addr.port_;
//	si.sin_addr.s_addr = addr.ip_;
//
//	auto bev = bufferevent_socket_new(eventBase_, -1, BEV_OPT_CLOSE_ON_FREE);
//	if (bev == nullptr)
//	{
//		return nullptr;
//	}
//
//	Channel* channel = new Channel(this, addr, bev, is_internal);
//	if (!addChannel(addr, channel))
//	{
//		delete channel;
//		bufferevent_free(bev);
//		return nullptr;
//	}
//	channel->setTimeout(channelTimeout_);
//
//	int bRet = bufferevent_socket_connect(bev, (struct sockaddr*)&si, sizeof(si));
//	if (0 != bRet)
//	{
//		return nullptr;
//	}
//
//	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)channel);
//	bufferevent_enable(bev, EV_READ | EV_WRITE);
//
//	event_set_log_callback(&CNet::log_cb);
//
//	return channel;
//}
//
//bool CNet::finalize()
//{
//    closeSocketAll();
//
//    if (extListener_)
//    {
//        evconnlistener_free(extListener_);
//		extListener_ = nullptr;
//    }
//
//	if (intListener_)
//	{
//		evconnlistener_free(intListener_);
//		intListener_ = nullptr;
//	}
//
//    if (eventBase_)
//    {
//        event_base_free(eventBase_);
//        eventBase_ = nullptr;
//    }
//
//    return true;
//}
//
//bool CNet::closeChannel(Channel* channel)
//{
//	if (channel == nullptr)
//	{
//		return false;
//	}
//
//	channel->setFlags(IChannel::FLAG_CONDEMN, true);
//	removeChannels_.push_back(channel->getAddr());
//
//	return true;
//}
//
//bool CNet::closeChannel(const Address& addr)
//{
//    auto it = channels_.find(addr);
//    if (it != channels_.end())
//    {
//		Channel* channel = it->second;
//
//		// 移除之前先回调
//		if (channelDeregisterCb_)
//		{
//			channelDeregisterCb_(channel);
//		}
//
//		struct ::bufferevent* bev = (struct ::bufferevent*)channel->getUserData();
//		bufferevent_free(bev);
//
//		channels_.erase(it);
//
//		delete channel;
//		channel = nullptr;
//
//        return true;
//    }
//
//    return false;
//}
//
//bool CNet::processChannels()
//{
//	for (auto it = channels_.begin(); it != channels_.end();)
//	{
//		Channel* channel = it->second;
//		const Address& addr = channel->getAddr();
//
//		if (channel->checkFlags(IChannel::FLAG_CONDEMN))
//		{
//			++it;
//			closeChannel(addr);
//		}
//		else if (channel->checkFlags(IChannel::FLAG_TIMEOUT))
//		{
//			++it;
//			onChannelTimeout(channel);
//		}
//		else
//		{
//			++it;
//			channel->updateTick();
//		}
//	}
//
//	return true;
//}
//
//void CNet::onChannelTimeout(Channel* channel)
//{
//	if (channelTimeoutCb_)
//	{
//		channelTimeoutCb_(channel);
//	}
//
//	closeChannel(channel);
//}
//
//bool CNet::addChannel(const Address& addr, Channel* channel)
//{
//	if (channel == nullptr)
//	{
//		return false;
//	}
//
//	return channels_.insert(std::make_pair(addr, channel)).second;
//}
//
//bool CNet::closeSocketAll()
//{
//	for (auto it = channels_.begin();  it != channels_.end(); ++it)
//	{
//		Channel* channel = it->second;
//
//		struct ::bufferevent* bev = (struct ::bufferevent*)channel->getUserData();
//
//		bufferevent_free(bev);
//
//		delete channel;
//		channel = nullptr;
//	}
//
//	removeChannels_.clear();
//    channels_.clear();
//
//    return true;
//}
//
//IChannel* CNet::getChannel(const Address& addr)
//{
//    auto it = channels_.find(addr);
//    if (it != channels_.end())
//    {
//        return it->second;
//    }
//
//    return nullptr;
//}
//
//void CNet::executeClose()
//{
//    for (size_t i = 0; i < removeChannels_.size(); ++i)
//    {
//		const Address& addr = removeChannels_[i];
//        closeChannel(addr);
//    }
//
//    removeChannels_.clear();
//}
//
//void CNet::log_cb(int severity, const char* msg)
//{
//
//}
//
//bool CNet::log(int severity, const char* msg)
//{
//    log_cb(severity, msg);
//    return true;
//}
//
//void CNet::handleMessage(Channel* channel, const char* data, size_t len, int msgid)
//{
//	if (channel == nullptr || data == nullptr || len <= 0)
//	{
//		return;
//	}
//
//	if (recvCb_)
//	{
//		recvCb_(channel, data, len, msgid);
//
//		receiveMsgTotal_++;
//	}
//}
//
//bool CNet::sendDataToAllClient(const char* data, const size_t len, int msgid)
//{
//	for (auto it = channels_.begin(); it != channels_.end(); ++it)
//	{
//		Channel* channel = it->second;
//		if (channel == nullptr)
//		{
//			continue;
//		}
//
//		if (!channel->checkFlags(IChannel::FLAG_CONDEMN)
//			&& channel->checkFlags(IChannel::FLAG_HANDSHAKE))
//		{
//			channel->sendData(data, len);
//		}
//		
//	}
//
//	return true;
//}
//
//
//
