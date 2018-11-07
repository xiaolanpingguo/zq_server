#pragma once

#include "lib/network/asio_fwd.hpp"
#include "IModule.h"

namespace boost {
	namespace asio {
		namespace ip {
			class tcp;
		}
	}
}
namespace zq {

class IChannel;
class MessageBuffer;
using ExtSocketMessgeCbT = std::function<bool(IChannel* channel, const void* data, size_t len)>;
using ExtPacketReaderHandleT = std::function<void(IChannel* channel, MessageBuffer& packet)>;
class IChannel
{
public:

	virtual void close() = 0;
	virtual bool sendData(const void* data, const size_t len) = 0;
	virtual const char* c_str() = 0;
	virtual const boost::asio::ip::tcp::endpoint& getAddr() = 0;
};
using IChannelPtr = std::shared_ptr<IChannel>;

}
