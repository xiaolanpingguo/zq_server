#pragma once


#include "socket.hpp"

namespace zq {


// һ������������ڲ���ַ�����õ�socket�����ﲻ�����ͻ��˲���
// eg:A�����ȥ��B�����ͬʱA����ֻ�ȥ�����������������(���ڲ���ַ������)
// ����A�����Ϊ�ͻ��˵Ĳ��ֲ��õ���ClientSocket(client.socket.hpp)
// ����A�����Ϊserver���������������ʱ�����ɵľ���InternalSocket
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

		// ͷ4���ֽڴ������ݳ���
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