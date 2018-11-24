#pragma once


#include "socket_mgr.hpp"
#include "connector.hpp"
#include "internal_socket.hpp"
#include "interface_header/base/INet.h"

namespace zq {


template<typename SockType>
class InternalSocketMgr : public SocketMgr<SockType>
{
	using BaseSocketMgr = SocketMgr<SockType>;
	using InternalSocketMapT = std::map<tcp_t::endpoint, InternalSocketPtr>;
public:
	InternalSocketMgr() {}
	virtual ~InternalSocketMgr()
	{
		intSocketMap_.clear();
		BaseSocketMgr::stopNetwork();
	}

	bool startNetwork(std::string const& bindIp, uint16 port, int threadCount = 1) override
	{
		if (!BaseSocketMgr::startNetwork(bindIp, port, threadCount))
			return false;

		BaseSocketMgr::acceptor_->setAccecptCb(std::bind(&InternalSocketMgr::onAccept, this, std::placeholders::_1, std::placeholders::_2));
		return true;
	}

	int run() override
	{
		int num = BaseSocketMgr::run();
		checkSocket();
		return num;
	}

	void checkSocket()
	{
		for (auto it = intSocketMap_.begin(); it != intSocketMap_.end();)
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
		}
	}

	void onAccept(tcp_t::socket&& sock, uint32 threadIndex)
	{
		onSocketOpen(std::move(sock));
	}

	void startConnect(const tcp_t::endpoint& ep, bool is_asyn = true)
	{
		//connector_.reset(new Connector(ioContext_));
		//connector_->startConnect(ep,
		//	[this](tcp_t::socket&& sock)
		//{
		//	this->onSocketOpen(std::move(sock));
		//	//connCb_(newSocket.get(), true);		
		//},
		//	[this](const boost::system::system_error& ec)
		//{
		//	//connCb_(nullptr, false);
		//	LOG_ERROR << "connect faid, err: " << ec.what();
		//}
		//, is_asyn);
	}

protected:

	void onSocketOpen(tcp_t::socket&& sock)
	{
		InternalSocketPtr newSocket = std::make_shared<SockType>(std::move(sock));
		if (BaseSocketMgr::socketSystemSendBufferSize_ > 0)
		{
			error_code_t err;
			sock.set_option(boost::asio::socket_base::send_buffer_size(BaseSocketMgr::socketSystemSendBufferSize_), err);
			if (err && err != boost::system::errc::not_supported)
			{
				LOG_ERROR << "InternalSocketMgr::OnSocketOpen sock.set_option err:" << err.message().c_str();
			}
		}
		newSocket->setNoDelay(true);
		newSocket->start();
		newSocket->asyncRead();
		this->intSocketMap_[newSocket->getEndpoint()] = newSocket;
	}

	bool removeSocket(InternalSocketPtr socket)
	{
		const tcp_t::endpoint& ep = socket->getEndpoint();
		auto it = intSocketMap_.find(ep);
		if (it != intSocketMap_.end())
		{
			it->second->onClose();
			intSocketMap_.erase(it);
			return true;
		}

		return false;
	}

protected:

	ConnectorPtr connector_;
	InternalSocketMapT intSocketMap_;
};

}