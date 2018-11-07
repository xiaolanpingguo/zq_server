#include "net_cs_module.h"
#include "packet_handler.h"


namespace zq {

NetCSModule::NetCSModule(ILibManager* p)
{
	using namespace std::placeholders;

    libManager_ = p;
	lastActTime_ = getLibManager()->getNowTime();
	socketSystemSendBufferSize_ = -1;

	//intSocketMgr_.setConnCb(std::bind(&NetCSModule::onIntSocketNetEvent, this, _1, _2));
}

NetCSModule::~NetCSModule()
{
	stopNetwork();
}

bool NetCSModule::init()
{
	logModule_ = libManager_->findModule<ILogModule>();
	return true;
}

int NetCSModule::startServer(const std::string& ip, uint16 port,
	int rbuffer, int wbuffer, int max_conn)
{
	if (!startNetwork(ip, port))
		return -1;

	acceptor_->setAccecptCb(std::bind(&NetCSModule::onAccept, this, std::placeholders::_1, std::placeholders::_2));
	return -1;
}

bool NetCSModule::run()
{
	update();
    return true;
}

bool NetCSModule::addChannel(IChannelPtr channel)
{
	const auto& ep = channel->getAddr();
	auto it = intSocketMap_.find(ep);
	if (it == intSocketMap_.end())
	{
		return false;
	}

	intSocketMap_[ep] = channel;
	return true;
}

void NetCSModule::onAccept(tcp::socket&& sock, uint32 threadIndex)
{
	using namespace std::placeholders;

	ExternalSocketPtr newSocket = std::make_shared<ExternalSocket>(std::move(sock));
	if (!addChannel(newSocket))
	{
		return;
	}

	if (socketSystemSendBufferSize_ > 0)
	{
		boost::system::error_code err;
		sock.set_option(boost::asio::socket_base::send_buffer_size(socketSystemSendBufferSize_), err);
		if (err && err != boost::system::errc::not_supported)
		{
			LOG_ERROR << "InternalSocketMgr::OnSocketOpen sock.set_option err:" << err.message().c_str();
		}
	}

	newSocket->setNoDelay(true);
	newSocket->setDataHandler(std::move(messageCb_));
	newSocket->setReadPacketHandler(std::move(packetReadHanlder_));
	newSocket->start();
	newSocket->asyncRead();
}

void NetCSModule::onChannelDataCb(IChannel* channel, const void* data, size_t len)
{
	/*if (channel)
	{
		if (channel->isInternal() && intChannelRemoveCb_)
		{
			intChannelRemoveCb_(channel);
		}
	}*/
}

void NetCSModule::onChannelTimeout(IChannel* channel)
{
	//logModule_->error(fmt::format("NetCSModule::onChannelTimeout, isInternal:{}, channal:{}.",
	//	channel->isInternal(), channel->c_str()));
}

}
