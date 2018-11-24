#pragma once 

#include "asio_fwd.hpp"
#include "message_buffer.hpp"
#include <atomic>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>

namespace zq {

static constexpr int INIT_READ_SIZE = 4096;
static constexpr int BUFFER_MAX_READ = 65535;


// 组件内部用的缓冲区大小和头部字节大小
static constexpr int INT_HEADER_LENTH = 4;
static constexpr int INT_BUFFER_MAX_READ = 65535;

#ifdef ASIO_HAS_IOCP
#define ZQ_SOCKET_USE_IOCP
#endif


template<typename SockType>
inline bool partialProcessPacket(SockType* handler, bool(SockType::*processMethod)(), MessageBuffer& outputBuffer, MessageBuffer& inputBuffer)
{
	MessageBuffer& buffer = outputBuffer;

	if (buffer.getRemainingSpace() > 0)
	{
		// readbuff下还有多少没读取的字节，输出buff还有剩下的多少空间
		std::size_t readDataSize = std::min(inputBuffer.getActiveSize(), buffer.getRemainingSpace());
		buffer.write(inputBuffer.getReadPointer(), readDataSize);
		inputBuffer.readCompleted(readDataSize);
	}

	if (buffer.getRemainingSpace() > 0)
	{
		// 还有剩下的字节没有收，等待下次再收.
		ASSERT(inputBuffer.getActiveSize() == 0);
		return false;
	}

	if (!(handler->*processMethod)())
	{
		handler->closeSocket();
		return false;
	}

	return true;
}

template<class T>
class Socket : public std::enable_shared_from_this<T>
{
public:
	explicit Socket(tcp_t::socket&& socket) : socket_(std::move(socket)), 
		readBuffer_(), closed_(false), closing_(false), isWritingAsync_(false)
	{
		error_code_t ec;
		endpoint_ = socket_.remote_endpoint(ec);
		if (!ec)
		{
			remoteAddress_ = endpoint_.address();
			remotePort_ = endpoint_.port();
		}
		else
		{
			LOG_ERROR << "constructor socket error, ec: " << ec.message();
		}

		readBuffer_.resize(INIT_READ_SIZE);
	}

	// for client
	explicit Socket(io_context_t& ioContext, const std::string& ip, uint16 port) :
		socket_(ioContext), readBuffer_(), closed_(false), closing_(false), isWritingAsync_(false)
	{
		error_code_t ec;
		remoteAddress_ = asio::ip::make_address(ip, ec);
		if (!ec)
		{
			endpoint_ = tcp_t::endpoint(remoteAddress_, port);
			remotePort_ = endpoint_.port();
		}
		else
		{
			LOG_ERROR << "constructor client socket error, ec: " << ec.message();
		}

		readBuffer_.resize(INIT_READ_SIZE);
	}

	virtual ~Socket()
	{
		closed_ = true;
		error_code_t error;
		socket_.close(error);
	}

	virtual void start() {};

	virtual bool update()
	{
		if (closed_)
			return false;

#ifndef ZQ_SOCKET_USE_IOCP
		if (isWritingAsync_ || (writeQueue_.empty() && !closing_))
			return true;

		for (; handleQueue();)
			;
#endif

		return true;
	}

	virtual bool sendData(const void* data, const size_t len)
	{
		if (data == nullptr || len <= 0)
		{
			return false;
		}

		MessageBuffer buffer;
		buffer.write(data, len);
		queuePacket(std::move(buffer));
		return true;
	}

	void asyncRead()
	{
		if (!isOpen())
			return;

		readBuffer_.normalize();
		readBuffer_.ensureFreeSpace();
		socket_.async_read_some(asio::buffer(readBuffer_.getWritePointer(), readBuffer_.getRemainingSpace()),
			std::bind(&Socket<T>::readHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void asyncReadWithCallback(void (T::*callback)(error_code_t, std::size_t))
	{
		if (!isOpen())
			return;

		readBuffer_.normalize();
		readBuffer_.ensureFreeSpace();
		socket_.async_read_some(asio::buffer(readBuffer_.getWritePointer(), readBuffer_.getRemainingSpace()),
			std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	bool isOpen() const { return !closed_ && !closing_; }

	void closeSocket()
	{
		if (closed_.exchange(true))
			return;

		error_code_t shutdownError;
		socket_.shutdown(asio::socket_base::shutdown_send, shutdownError);
		onClose();
	}

	void setNoDelay(bool enable)
	{
		error_code_t err;
		socket_.set_option(tcp::no_delay(enable), err);
	}

	// Marks the socket for closing after write buffer becomes empty
	void delayedCloseSocket() { closing_ = true; }

	MessageBuffer& getReadBuffer() { return readBuffer_; }

	tcp_t::socket& getSocket() { return socket_; }

	const tcp_t::endpoint& getEndpoint() const { return endpoint_; }
	std::string getIp() const 
	{
		error_code_t ec;
		return remoteAddress_.to_string(ec);
	}
	uint16 getPort() const { return remotePort_; }
	const char* c_str()
	{
		if (addressStr_.empty())
		{
			addressStr_ = getIp() + ":" + std::to_string(getPort());
		}
		return addressStr_.c_str();
	}

	virtual void onClose() { }

protected:

	virtual void readHandler() = 0;

	void queuePacket(MessageBuffer&& buffer)
	{
		writeQueue_.push(std::move(buffer));

#ifdef ZQ_SOCKET_USE_IOCP
		asyncProcessQueue();
#endif
	}

	bool asyncProcessQueue()
	{
		if (isWritingAsync_)
			return false;

		isWritingAsync_ = true;

#ifdef ZQ_SOCKET_USE_IOCP
		MessageBuffer& buffer = writeQueue_.front();
		socket_.async_write_some(asio::buffer(buffer.getReadPointer(), buffer.getActiveSize()), std::bind(&Socket<T>::writeHandler,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#else
		socket_.async_write_some(asio::null_buffers(), std::bind(&Socket<T>::writeHandlerWrapper,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

		return false;
	}

private:
	void readHandlerInternal(error_code_t error, size_t transferredBytes)
	{
		if (error)
		{
			closeSocket();
			return;
		}

		if (!isOpen())
		{
			return;
		}

		readBuffer_.writeCompleted(transferredBytes);
		readHandler();
	}

#ifdef ZQ_SOCKET_USE_IOCP

	void writeHandler(error_code_t error, std::size_t transferedBytes)
	{
		if (!error)
		{
			isWritingAsync_ = false;
			writeQueue_.front().readCompleted(transferedBytes);
			if (!writeQueue_.front().getActiveSize())
				writeQueue_.pop();

			if (!writeQueue_.empty())
				asyncProcessQueue();
			else if (closing_)
				closeSocket();
		}
		else
			closeSocket();
	}

#else

	void writeHandlerWrapper(error_code_t /*error*/, std::size_t /*transferedBytes*/)
	{
		isWritingAsync_ = false;
		handleQueue();
	}

	bool handleQueue()
	{
		if (writeQueue_.empty())
			return false;

		MessageBuffer& queuedMessage = writeQueue_.front();

		std::size_t bytesToSend = queuedMessage.getActiveSize();

		error_code_t error;
		std::size_t bytesSent = socket_.write_some(asio::buffer(queuedMessage.getReadPointer(), bytesToSend), error);
		if (error)
		{
			if (error == asio::error::would_block || error == asio::error::try_again)
				return asyncProcessQueue();

			writeQueue_.pop();
			if (closing_ && writeQueue_.empty())
				closeSocket();

			return false;
		}
		else if (bytesSent == 0)
		{
			writeQueue_.pop();
			if (closing_ && writeQueue_.empty())
				closeSocket();

			return false;
		}
		else if (bytesSent < bytesToSend) // now n > 0
		{
			queuedMessage.readCompleted(bytesSent);
			return asyncProcessQueue();
		}

		writeQueue_.pop();
		if (closing_ && writeQueue_.empty())
			closeSocket();

		return !writeQueue_.empty();
	}

#endif

	tcp_t::socket socket_;

	tcp_t::endpoint endpoint_;
	address_t remoteAddress_;
	uint16 remotePort_;
	std::string addressStr_;

	MessageBuffer readBuffer_;
	std::queue<MessageBuffer> writeQueue_;

	std::atomic<bool> closed_;
	std::atomic<bool> closing_;

	bool isWritingAsync_;
};

}

