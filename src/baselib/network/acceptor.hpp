#pragma once

#include "interface_header/base/platform.h"
#include "asio_fwd.hpp"
#include <functional>
#include <atomic>

namespace zq {

#define MAX_LISTEN_CONNECTIONS asio::socket_base::max_connections

class AsyncAcceptor
{
public:
    using AcceptCallbackT = std::function<void(tcp_t::socket&& newSocket, uint32 threadIndex)>;

    AsyncAcceptor(io_context_t& ioContext, std::string const& bindIp, uint16 port) :
        acceptor_(ioContext), endpoint_(address_t::from_string(bindIp), port),
        socket_(ioContext), closed_(false), socketFactory_(std::bind(&AsyncAcceptor::defeaultSocketFactory, this))
    {
    }

    template<class T>
	void asyncAccept()
	{
		acceptor_.async_accept(socket_, [this](error_code_t error)
		{
			if (!error)
			{
				try
				{
					// this-> is required here to fix an segmentation fault in gcc 4.7.2 - reason is lambdas in a templated class
					std::make_shared<T>(std::move(this->socket_))->start();
				}
				catch (const sys_err_t& err)
				{
					LOG_ERROR << "Failed to retrieve client's remote address, err: %s" << err.what();
				}
			}

			// lets slap some more this-> on this so we can fix this bug with gcc 4.7.2 throwing internals in yo face
			if (!closed_)
				this->asyncAccept<T>();
		});
	}

    void asyncAcceptWithCallback()
    {
		tcp_t::socket* socket;
        uint32 threadIndex;
        std::tie(socket, threadIndex) = socketFactory_();
        acceptor_.async_accept(*socket, [this, socket, threadIndex](const error_code_t& error)
        {
            if (!error)
            {
                try
                {
                    socket->non_blocking(true);

					if (acceptCb_)
						acceptCb_(std::move(*socket), threadIndex);
                }
                catch (const sys_err_t& err)
                {
                    LOG_ERROR << "Failed to initialize client's socket, err: " << err.what();
                }
            }

            if (!closed_)
                this->asyncAcceptWithCallback();
        });
    }

    bool bind()
    {
		error_code_t errorCode;
        acceptor_.open(endpoint_.protocol(), errorCode);
        if (errorCode)
        {
            LOG_ERROR << "Failed to open acceptor, errormsg: " << errorCode.message().c_str();
            return false;
        }

        acceptor_.bind(endpoint_, errorCode);
        if (errorCode)
        {
			LOG_ERROR << "Could not bind to ip:" << endpoint_.address().to_string().c_str() <<" port: "<< 
				endpoint_.port() << " errormsg: " << errorCode.message().c_str();
            return false;
        }

        acceptor_.listen(MAX_LISTEN_CONNECTIONS, errorCode);
        if (errorCode)
        {
			LOG_ERROR << "_acceptor.listen error, errormsg: " << errorCode.message().c_str();
            return false;
        }

        return true;
    }

    void close()
    {
        if (closed_.exchange(true))
            return;

        error_code_t err;
        acceptor_.close(err);
    }

    void setSocketFactory(std::function<std::pair<tcp_t::socket*, uint32>()>&& func) { socketFactory_ = std::move(func); }
	void setAccecptCb(AcceptCallbackT&& cb) { acceptCb_ = std::move(cb); }

private:
    std::pair<tcp_t::socket*, uint32> defeaultSocketFactory() { return std::make_pair(&socket_, 0); }

	tcp_t::acceptor acceptor_;
	tcp_t::endpoint endpoint_;
	tcp_t::socket socket_;
    std::atomic<bool> closed_;
    std::function<std::pair<tcp_t::socket*, uint32>()> socketFactory_;
	AcceptCallbackT acceptCb_;;
};


}

