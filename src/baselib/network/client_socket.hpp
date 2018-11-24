#pragma once

#include "socket.hpp"

namespace zq {


// 作为客户端的套接字，这里默认已经给出了拆包解包规则
// 如果不想用默认规则，重写readHandler即可
class ClientSocket : public Socket<ClientSocket>
{
public:
	using BaseSocket = Socket<ClientSocket>;

	ClientSocket(io_context_t& ios, const std::string& ip, uint16 port) 
		: Socket(ios, ip, port),
		ioContext_(ios)
	{
		headerLengthBuffer_.resize(INT_HEADER_LENTH);
		packetBuffer_.resize(0);
	}

	virtual ~ClientSocket()
	{
	}

	bool update() override
	{
		if (!BaseSocket::update())
		{
			return false;
		}

		error_code_t ec;
		ioContext_.poll(ec);
		return true;
	}

	void asynConnect()
	{
		getSocket().async_connect(getEndpoint(), [this](const error_code_t& ec)
		{
			if (!ec)
			{
				this->start();
				this->asyncRead();
			}
			else
			{
				this->closeSocket();
				LOG_ERROR << "connect faild, ip: " << getIp() << " port: " << getPort() << " err: " << ec.message();
			}
		});
	}

	void synConnect()
	{
		while (1)
		{
			update();

			error_code_t ec;
			getSocket().connect(getEndpoint(), ec);
			if (!ec)
			{
				break;
			}
		}

		start();
		asyncRead();
	}

	bool isConnect()
	{
		return isOpen();
	}

	void readHandler() override
	{
		if (!isOpen())
			return;

		MessageBuffer& packet = getReadBuffer();

		while (packet.getActiveSize() > 0)
		{
			if (!partialProcessPacket<ClientSocket>(this, &ClientSocket::readHeaderLengthHandler, headerLengthBuffer_, packet))
				break;

			if (!partialProcessPacket<ClientSocket>(this, &ClientSocket::readDataHandler, packetBuffer_, packet))
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

	std::string getIp() { return BaseSocket::getIp(); }
	uint16 getPort() { return BaseSocket::getPort(); }

protected:

	MessageBuffer headerLengthBuffer_;
	MessageBuffer packetBuffer_;

	io_context_t& ioContext_;
};


}

