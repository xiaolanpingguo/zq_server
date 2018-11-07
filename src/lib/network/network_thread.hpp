#pragma once

#include "lib/interface_header/platform.h"
#include "asio_fwd.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

namespace zq {


using boost::asio::ip::tcp;

template<class SocketType>
class NetworkThread
{
public:
    NetworkThread() 
		: connections_(0), stopped_(false), thread_(nullptr), ioContext_(1),
        acceptSocket_(ioContext_), updateTimer_(ioContext_)
    {
    }

    virtual ~NetworkThread()
    {
        stop();
        if (thread_)
        {
            wait();
            delete thread_;
        }
    }

    bool start()
    {
        if (thread_)
            return false;

        thread_ = new std::thread(&NetworkThread::run, this);
        return true;
    }

	void stop()
	{
		stopped_ = true;
		ioContext_.stop();
	}

    void wait()
    {
		if (thread_)
		{
			thread_->join();
			delete thread_;
			thread_ = nullptr;
		}
    }

    virtual void addSocket(std::shared_ptr<SocketType> sock)
    {
        std::lock_guard<std::mutex> lock(newSocketsLock_);

        ++connections_;
        newSockets_.push_back(sock);
        socketAdded(sock);
    }

	int32 getConnectionCount() const { return connections_; }

    tcp::socket* getSocketForAccept() { return &acceptSocket_; }

protected:
    virtual void socketAdded(std::shared_ptr<SocketType> /*sock*/) { }
    virtual void socketRemoved(std::shared_ptr<SocketType> /*sock*/) { }

    void addNewSockets()
    {
        std::lock_guard<std::mutex> lock(newSocketsLock_);

        if (newSockets_.empty())
            return;

        for (std::shared_ptr<SocketType> sock : newSockets_)
        {
            if (!sock->isOpen())
            {
                socketRemoved(sock);
                --connections_;
            }
            else
                sockets_.push_back(sock);
        }

        newSockets_.clear();
    }

    void run()
    {
        updateTimer_.expires_from_now(boost::posix_time::milliseconds(10));
        updateTimer_.async_wait(std::bind(&NetworkThread<SocketType>::update, this));
        ioContext_.run();
        newSockets_.clear();
        sockets_.clear();
    }

    void update()
    {
        if (stopped_)
            return;

        updateTimer_.expires_from_now(boost::posix_time::milliseconds(10));
        updateTimer_.async_wait(std::bind(&NetworkThread<SocketType>::update, this));

        addNewSockets();

        sockets_.erase(std::remove_if(sockets_.begin(), sockets_.end(), [this](std::shared_ptr<SocketType> sock)
        {
            if (!sock->update())
            {
                if (sock->isOpen())
                    sock->closeSocket();

                this->socketRemoved(sock);

                --this->connections_;
                return true;
            }

            return false;
        }), sockets_.end());
    }

private:
    typedef std::vector<std::shared_ptr<SocketType>> SocketContainer;

    std::atomic<int32> connections_;
    std::atomic<bool> stopped_;

    std::thread* thread_;

    SocketContainer sockets_;

    std::mutex newSocketsLock_;
    SocketContainer newSockets_;

    Asio::IoContext ioContext_;
    tcp::socket acceptSocket_;
    boost::asio::deadline_timer updateTimer_;
};

}
