#pragma once

#include "acceptor.hpp"
#include "network_thread.hpp"
#include "connector.hpp"
#include <memory>

namespace zq {


struct NET_SINGLE_THREAD{};
struct NET_MULTi_THREAD{};

template<typename SocketType, typename ThreadMod = NET_SINGLE_THREAD>
class SocketMgr
{
	using SocketMapT = std::map<tcp::endpoint, std::shared_ptr<SocketType>>;
protected:
	SocketMgr()
		: acceptor_(nullptr),
		threads_(nullptr),
		threadCount_(0),
		ioContext_(Asio::getDefaultIoContextObj())
	{
	}

	virtual ~SocketMgr()
	{
		ASSERT(!threads_ && !acceptor_ && !threadCount_, "StopNetwork must be called prior to SocketMgr destruction");
	}

	virtual NetworkThread<SocketType>* createThreads() const { return new NetworkThread<SocketType>[1]; }

	virtual int update()
	{
		updateSocket();
		boost::system::error_code ec;
		size_t num = ioContext_.poll(ec);
		return (int)num;
	}

	virtual bool startNetwork(std::string const& bindIp, uint16 port, int threadCount = 1)
	{
		ASSERT(threadCount > 0);

		AsyncAcceptor* acceptor = nullptr;
		try
		{
			acceptor = new AsyncAcceptor(ioContext_, bindIp, port);
		}
		catch (boost::system::system_error const& err)
		{
			LOG_ERROR << "Exception caught in SocketMgr.StartNetwork,ip: " << 
				bindIp.c_str() << " ,port port" << port << " ,errmsg:" << err.what();
			return false;
		}

		if (!acceptor->bind())
		{
			LOG_ERROR <<  "StartNetwork failed to bind socket acceptor";
			return false;
		}

		acceptor_ = acceptor;
		acceptor_->setAccecptCb(std::bind(&SocketMgr<SocketType, ThreadMod>::onSocketOpen, this, std::placeholders::_1, std::placeholders::_2));
		threadCount_ = threadCount;
		threads_ = createThreads();

		ASSERT(threads_);

		for (int32 i = 0; i < threadCount_; ++i)
			threads_[i].start();

		ip_ = bindIp;
		port_ = port;

		acceptor_->asyncAcceptWithCallback();

		return true;
	}

	virtual void stopNetwork()
	{
		acceptor_->close();
		ioContext_.stop();

		if (threadCount_ != 0)
			for (int32 i = 0; i < threadCount_; ++i)
				threads_[i].stop();

		wait();

		if (acceptor_)
		{
			delete acceptor_;
			acceptor_ = nullptr;
		}

		if (threads_)
		{
			delete[] threads_;
			threads_ = nullptr;
		}

		threadCount_ = 0;
	}

	void wait()
	{
		if (threadCount_ != 0)
			for (int32 i = 0; i < threadCount_; ++i)
				threads_[i].wait();
	}

	virtual void onSocketOpen(tcp::socket&& sock, uint32 threadIndex)
	{
		try
		{
			std::shared_ptr<SocketType> newSocket = std::make_shared<SocketType>(std::move(sock));
			newSocket->setNoDelay(true);
			newSocket->start();
			newSocket->asyncRead();
			addSocket(ThreadMod(), newSocket, threadIndex);
		}
		catch (boost::system::system_error const& err)
		{
			LOG_ERROR << "Failed to retrieve client's remote address: %s" << err.what();
		}
	}

	uint32 selectThreadWithMinConnections() const
	{
		uint32 min = 0;

		for (int32 i = 1; i < threadCount_; ++i)
			if (threads_[i].getConnectionCount() < threads_[min].getConnectionCount())
				min = i;

		return min;
	}

	std::pair<tcp::socket*, uint32> getSocketForAccept()
	{
		uint32 threadIndex = selectThreadWithMinConnections();
		return std::make_pair(threads_[threadIndex].getSocketForAccept(), threadIndex);
	}	

	void addSocket(NET_SINGLE_THREAD, std::shared_ptr<SocketType> newSocket, uint32 threadIndex)
	{
		socketMap_[newSocket->getEndpoint()] = newSocket;
	}

	void addSocket(NET_MULTi_THREAD, std::shared_ptr<SocketType> newSocket, uint32 threadIndex)
	{
		threads_[threadIndex].addSocket(newSocket);
	}

	void updateSocket()
	{
		for (auto it = socketMap_.begin(); it != socketMap_.end();)
		{
			auto sock = it->second;
			if (!sock->update())
			{
				it = socketMap_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	int32 getNetworkThreadCount() const { return threadCount_; }

	const std::string& getIp() { return ip_; }
	uint16 getPort() { return port_; }

protected:

	AsyncAcceptor* acceptor_;
	NetworkThread<SocketType>* threads_;
	int32 threadCount_;
	Asio::IoContext& ioContext_;

	std::string ip_;
	uint16 port_;

	SocketMapT socketMap_;
};


}

