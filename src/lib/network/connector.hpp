#pragma once

#include "asio_fwd.hpp"
#include <functional>

namespace zq{


using OnSuccessFunT = std::function<void(tcp::socket&& socket)>;
using OnFaildFunT = std::function<void(const boost::system::error_code&)>;
class Connector
{
public:
	Connector(Asio::IoContext& ios): socket_(ios) {}
	~Connector() = default;

	void startConnect(const tcp::endpoint& endpoint, OnSuccessFunT&& on_success, OnFaildFunT&& on_fail, bool async = true)
	{
		onSuccseeCb_ = std::move(on_success);
		onFaildCb_ = std::move(on_fail);
		if (async)
		{
			socket_.async_connect(endpoint, [this](const boost::system::error_code& ec)
			{
				if (!ec)
				{
					if (this->onSuccseeCb_)
					{
						this->onSuccseeCb_(std::move(socket_));
					}
				}
				else
				{
					if (this->onFaildCb_)
					{
						this->onFaildCb_(ec);
					}
				}
			});
		}
		else
		{
			boost::system::error_code ec;
			socket_.connect(endpoint, ec);
			if (!ec)
			{
				if (this->onSuccseeCb_)
				{
					this->onSuccseeCb_(std::move(socket_));
				}
			}
			else
			{
				if (this->onFaildCb_)
				{
					this->onFaildCb_(ec);
				}
			}
		}
	}

	tcp::socket&& socket()
	{
		return std::move(socket_);
	}

private:

	tcp::socket	socket_;
	OnSuccessFunT onSuccseeCb_;
	OnFaildFunT onFaildCb_;
};
using ConnectorPtr = std::unique_ptr<Connector>;

} 