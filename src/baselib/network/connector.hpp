#pragma once

#include "asio_fwd.hpp"
#include <functional>

namespace zq{


using OnSuccessFunT = std::function<void(tcp::socket&& socket)>;
using OnFaildFunT = std::function<void(const error_code_t&)>;
class Connector
{
public:
	Connector(io_context_t& ios): socket_(ios) {}
	~Connector() = default;

	void startConnect(const tcp::endpoint& endpoint, OnSuccessFunT&& on_success, OnFaildFunT&& on_fail, bool async = true)
	{
		onSuccseeCb_ = std::move(on_success);
		onFaildCb_ = std::move(on_fail);
		if (async)
		{
			socket_.async_connect(endpoint, [this](const error_code_t& ec)
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
			error_code_t ec;
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