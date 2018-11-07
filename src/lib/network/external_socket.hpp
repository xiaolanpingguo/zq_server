#pragma once


#include "socket.hpp"
#include "lib/interface_header/INet.h"

namespace zq {

class ExternalSocket;

template<bool(zq::ExternalSocket::*processMethod)(), MessageBuffer zq::ExternalSocket::*outputBuffer>
inline bool partialProcessPacket(ExternalSocket* handler, MessageBuffer& inputBuffer)
{
	MessageBuffer& buffer = handler->*outputBuffer;

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

	// 我们收到了这个buffer所需要的字节数
	if (!(handler->*processMethod)())
	{
		return false;
	}

	return true;
}

class ExternalSocket;
class IChannel;
using ExternalSocketPtr = std::shared_ptr<ExternalSocket>;
class ExternalSocket : public Socket<ExternalSocket>, public IChannel
{
public:
	ExternalSocket(tcp::socket&& socket)
		:Socket(std::move(socket))
	{
		headerLengthBuffer_.resize(HEADER_LENTH);
		packetBuffer_.resize(0);
	}
	virtual ~ExternalSocket() = default;

public:
	
	void start()
	{

	}

	void close()  override
	{
		closeSocket();
	}

	bool sendData(const void* data, const size_t len) override
	{
		return true;
	}

	const char* c_str() override
	{
		return c_str();
	}

	const tcp::endpoint& getAddr() override
	{
		return getEndpoint();
	}

	void readHandler() override
	{
		if (!isOpen())
		{
			return;
		}

		MessageBuffer& packet = getReadBuffer();

		if (packetReadHandler_)
		{
			packetReadHandler_(this, packet);
			return;
		}

		while (packet.getActiveSize() > 0)
		{
			if (!partialProcessPacket<&ExternalSocket::readHeaderLengthHandler, &ExternalSocket::headerLengthBuffer_>(this, packet))
				break;

			if (!partialProcessPacket<&ExternalSocket::readDataHandler, &ExternalSocket::packetBuffer_>(this, packet))
				break;

			headerLengthBuffer_.reset();
			packetBuffer_.reset();
		}

		asyncRead();
	}

	bool readHeaderLengthHandler()
	{
		uint32 data_len = ::ntohl(*(uint32*)headerLengthBuffer_.getBasePointer());
		if (data_len > BUFFER_MAX_READ)
		{
			LOG_ERROR << "close channel, read headlenth error, datalen: " << data_len;
			closeSocket();
			return false;
		}

		packetBuffer_.resize(data_len);
		return true;
	}

	virtual bool readDataHandler()
	{
		if (dataHandler_)
		{
			dataHandler_(this, packetBuffer_.getBasePointer(), packetBuffer_.getBufferSize());
		}

		return true;
	}

	void setDataHandler(ExtSocketMessgeCbT&& cb) { dataHandler_ = std::move(cb); }
	void setReadPacketHandler(ExtPacketReaderHandleT&& cb) { packetReadHandler_ = std::move(cb); }

private:

	constexpr static int HEADER_LENTH = 4;
	constexpr static int BUFFER_MAX_READ = 65535;

	MessageBuffer headerLengthBuffer_;
	MessageBuffer packetBuffer_;

	ExtSocketMessgeCbT dataHandler_;
	ExtPacketReaderHandleT packetReadHandler_;
};

}