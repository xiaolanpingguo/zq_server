#include "net_ss_module.h"


namespace zq{


NetSSModule::NetSSModule(ILibManager* p)
{
	using namespace std::placeholders;

    libManager_ = p;
	socketSystemSendBufferSize_ = -1;
}

NetSSModule::~NetSSModule()
{
	stopNetwork();
}

bool NetSSModule::init()
{
	logModule_ = libManager_->findModule<ILogModule>();
	return true;
}

void NetSSModule::connect(const std::string& ip, uint16 port, void* user_data, bool is_async)
{
	/*boost::system::error_code ec;
	auto addr = boost::asio::ip::make_address(ip, ec);
	if (ec)
	{
		logModule_->error(fmt::format("InternalSocketMgr::connect error, ip:{}, port:{}", ip.c_str(), port));
		return;
	}

	tcp::endpoint ep{ addr, port };

	connector_.reset(new Connector(ioContext_));
	connector_->startConnect(ep,
		[this, user_data](tcp::socket&& sock)
	{																															
		InternalSocketPtr newsock = this->newSocket(std::move(sock));
		if (newsock)
		{
			this->onNetEvent(newsock, true, user_data);
			newsock->asyncRead();
		}
	},
		[this, user_data](const boost::system::system_error& ec)
	{
		this->onNetEvent(nullptr, false, user_data);
	}
	, is_async);*/
}

int NetSSModule::startServer(const std::string& ip, uint16 port,
	int rbuffer, int wbuffer, int max_conn)
{
	if (!startNetwork(ip, port))
		return -1;

	acceptor_->setAccecptCb(std::bind(&NetSSModule::onAccept, this, std::placeholders::_1, std::placeholders::_2));
	return -1;
}

bool NetSSModule::run()
{
	update();
    return true;
}

bool NetSSModule::addChannel(InternalSocketPtr channel)
{
	//const auto& addr = channel->getAddr();
	//auto it = intSocketMap_.find(addr);
	//if (it == intSocketMap_.end())
	//{
	//	return false;
	//}

	//intSocketMap_[addr] = channel;
	return true;
}

void NetSSModule::onAccept(tcp::socket&& sock, uint32 threadIndex)
{
	InternalSocketPtr newsock = newSocket(std::move(sock));
	if (newsock)
	{
		onNetEvent(newsock, true, nullptr);
		newsock->asyncRead();
	}
}

InternalSocketPtr NetSSModule::newSocket(tcp::socket&& sock)
{
	if (socketSystemSendBufferSize_ > 0)
	{
		boost::system::error_code err;
		sock.set_option(boost::asio::socket_base::send_buffer_size(socketSystemSendBufferSize_), err);
		if (err && err != boost::system::errc::not_supported)
		{
			logModule_->error(fmt::format("InternalSocketMgr::OnSocketOpen sock.set_option err:{}.", err.message().c_str()));
		}
	}

	InternalSocketPtr newsock = std::make_shared<InternalSocket>(std::move(sock));
	if (!addChannel(newsock))
	{
		return nullptr;
	}
	newsock->setNoDelay(true);
	//newsock->setDataHandler(std::move(messageCb_));
	newsock->start();

	return newsock;
}

void NetSSModule::onNetEvent(InternalSocketPtr channel, bool conn, void* user_data)
{
	/*if (this->eventCb_)
	{
		this->eventCb_(channel.get(), conn, user_data);
	}*/
}

void NetSSModule::checkSocket()
{
	/*for (auto it = intSocketMap_.begin(); it != intSocketMap_.end();)
	{
		InternalSocketPtr socket = it->second;

		if (!socket->update())
		{
			++it;
			removeSocket(socket);
		}
		else
		{
			++it;
		}
	}*/
}

bool NetSSModule::removeSocket(InternalSocketPtr socket)
{
	//const tcp::endpoint& ep = socket->getEndpoint();
	//auto it = intSocketMap_.find(ep);
	//if (it != intSocketMap_.end())
	//{
	//	it->second->onClose();
	//	onNetEvent(it->second, false, nullptr);
	//	intSocketMap_.erase(it);
	//	return true;
	//}

	return false;
}

void NetSSModule::onChannelDataCb(IChannel* channel, const void* data, size_t len)
{
	/*if (channel)
	{
		if (channel->isInternal() && intChannelRemoveCb_)
		{
			intChannelRemoveCb_(channel);
		}
	}*/
}

void NetSSModule::onChannelTimeout(IChannel* channel)
{
	//logModule_->error(fmt::format("NetSSModule::onChannelTimeout, isInternal:{}, channal:{}.",
	//	channel->isInternal(), channel->c_str()));
}

}
