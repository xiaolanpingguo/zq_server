#pragma once

#include "connector.hpp"
#include <memory>

namespace zq {


template<typename SocketType>
class ClientSocketMgr
{
	using SocketMapT = std::map<tcp::endpoint, std::shared_ptr<SocketType>>;
public:
	ClientSocketMgr() : ioContext_(getDefaultIoContextObj())
	{
	}

	virtual ~ClientSocketMgr()
	{
		stop();
	}

	virtual int poll()
	{
		error_code_t ec;
		size_t num = ioContext_.poll(ec);
		if (ec)
		{
			LOG_ERROR << "run error!, errmsg: " << ec.message();
			return -1;
		}

		return (int)num;
	}

	virtual void stop()
	{
		ioContext_.stop();
	}

	template<typename... Agrs>
	void startConnect(const std::string& ip, uint16 port, bool is_asyn, Agrs... args)
	{
		error_code_t ec;
		auto addr = boost::asio::ip::make_address(ip, ec);
		if (ec)
		{
			LOG_ERROR << "connect faild, ip: " << ip << " port: " << port;
			return;
		}

		tcp::endpoint ep{ addr, port };
		connector_.reset(new Connector(ioContext_));
		connector_->startConnect(ep,
			[=](tcp::socket&& sock)
		{
			try
			{
				std::shared_ptr<SocketType> newSocket = std::make_shared<SocketType>(std::move(sock), args...);
				newSocket->setNoDelay(true);
				newSocket->start();	 
				newSocket->asyncRead();
				this->addSocket(newSocket);
			}
			catch (boost::system::system_error const& err)
			{
				LOG_ERROR << "startConnect occerd error, err: %s" << err.what();
			}
		},
			[=](const boost::system::system_error& ec)
		{
			LOG_ERROR << "connect faild, ip: "<< ip << " port: " << port << " err: " << ec.what();
		}
		, is_asyn);
	}	

	void addSocket(std::shared_ptr<SocketType> newSocket)
	{
		socketMap_[newSocket->getEndpoint()] = newSocket;
	}

protected:

	ConnectorPtr connector_;
	SocketMapT socketMap_;

	io_context_t& ioContext_;
};


}

