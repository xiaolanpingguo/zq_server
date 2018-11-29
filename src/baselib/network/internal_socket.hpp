#pragma once


#include "socket.hpp"

namespace zq {


// 一个组件监听的内部地址所采用的socket，这里不包含客户端部分
// eg:A组件会去连B组件，同时A组件又会去监听其他组件的连接(用内部地址来监听)
// 这里A组件作为客户端的部分采用的是ClientSocket(client.socket.hpp)
// 这里A组件作为server监听其他组件连接时，生成的就是InternalSocket
class InternalSocket;
using InternalSocketPtr = std::shared_ptr<InternalSocket>;
class InternalSocket : public Socket<InternalSocket>
{
public:
	InternalSocket(tcp_t::socket&& socket)
		:Socket(std::move(socket))
	{
		headerLengthBuffer_.resize(INT_HEADER_LENTH);
		packetBuffer_.resize(0);
	}
	virtual ~InternalSocket() = default;

public:

	virtual void readHandler() override
	{
		if (!isOpen())
			return;

		MessageBuffer& packet = getReadBuffer();

		while (packet.getActiveSize() > 0)
		{
			if (!partialProcessPacket<InternalSocket>(this, &InternalSocket::readHeaderLengthHandler, headerLengthBuffer_, packet))
				break;

			if (!partialProcessPacket<InternalSocket>(this, &InternalSocket::readDataHandler, packetBuffer_, packet))
				break;

			headerLengthBuffer_.reset();
			packetBuffer_.reset();
		}

		asyncRead();
	}

	bool readHeaderLengthHandler()
	{
		uint32 data_len = ::ntohl(*(uint32*)headerLengthBuffer_.getBasePointer());
		if (data_len > INT_BUFFER_MAX_READ)
		{
			LOG_ERROR << "close socket, read headlenth error, datalen: " << data_len;
			return false;
		}

		packetBuffer_.resize(data_len);
		return true;
	}

	virtual bool readDataHandler()
	{
		return true;
	}

	bool sendData(const void* data, const size_t len) override
	{
		if (data == nullptr || len <= 0)
		{
			return false;
		}

		// 头4个字节代表数据长度
		uint32 all_len = (uint32)len;
		all_len = ::htonl(all_len);

		MessageBuffer buffer;
		buffer.write((char*)&all_len, INT_HEADER_LENTH);
		buffer.write(data, len);
		queuePacket(std::move(buffer));

		return true;
	}

protected:

	MessageBuffer headerLengthBuffer_;
	MessageBuffer packetBuffer_;
};

}