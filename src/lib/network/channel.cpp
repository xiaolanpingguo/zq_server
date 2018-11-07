//#include "channel.h"
//#include "packet_handler.h"
//#include "net.h"
//#include "lib/base_code/util.h"
//#include "lib/base_code/format.h"
//#include "lib/base_code/sha1.h"
//#include "lib/base_code/base64.h"
//
//#include <unordered_map>
//#include <algorithm>
//using namespace zq;
//
//
//Channel::Channel(CNet* pNet, const Address& addr, void* pBev, bool is_internal)
//	: IChannel(),
//	inet_(pNet),
//	addr_(addr),
//	userData_(pBev),
//	needRemove_(false),
//	channelType_(CHANNEL_NORMALSOCKET),
//	sendPakcetNum_(0),
//	recvPakcetNum_(0),
//	channelTimeout_(60),
//	lastActTime_(time(nullptr)),
//	internal_(is_internal)
//{
//
//	//if (internal_)
//	//{
//	//	packetHandler_ = std::make_shared<ComponentPacketHandler>(this);
//	//}
//	//else
//	//{
//	//	packetHandler_ = std::make_shared<PacketHandler>(this);
//	//}
//}
//
//Channel::~Channel()
//{
//
//}
//
//INet* Channel::getNet()
//{
//	return inet_;
//}
//
//void Channel::close()
//{
//	inet_->closeChannel(this);
//}
//
//const char* Channel::c_str()
//{
//	return addr_.c_str();
//}
//
//bool Channel::sendData(const void* data, const size_t len, int msgid, bool encode)
//{
//	// 已经非法了，或者还未连接成功
//	if (checkFlags(IChannel::FLAG_CONDEMN) || !checkFlags(IChannel::FLAG_RUNNING))
//	{
//		return false;
//	}
//
//	bufferevent* bev = (bufferevent*)getUserData();
//	if (bev)
//	{
//		if (encode)
//		{
//			static std::string out_data;
//			out_data.clear();
//			enCode(data, len, out_data, msgid);
//			bufferevent_write(bev, out_data.c_str(), out_data.size());
//		}
//		else
//		{
//			bufferevent_write(bev, data, len);
//		}
//
//		sendPakcetNum_++;
//	}
//
//	return true;
//}
//
//void Channel::updateTick()
//{
//	// 这里检测超时的通道
//	if (channelTimeout_ <= 0)
//	{
//		return;
//	}
//
//	time_t now = time(nullptr);
//	if (lastActTime_ + channelTimeout_ < now)
//	{
//		if (checkFlags(FLAG_RUNNING))
//		{
//			LOG_ERROR << "channel timeout, close channel, channel: " << c_str();
//			this->setFlags(FLAG_TIMEOUT, true);
//		}
//	}
//}
//
//size_t Channel::addBuff(const char* str, size_t nLen)
//{
//	lastActTime_ = time(nullptr);
//
//	size_t remaining_size = readBuffer_.getRemainingSpace();
//	size_t write_size = std::min(remaining_size, nLen);
//	readBuffer_.write(str, write_size);
//	return write_size;
//}
//
//int Channel::enCode(const void* strData, size_t unDataLen, std::string& strOutData, int msgid)
//{
//	// 头4个字节代表数据长度
//	uint32 all_len = (uint32)unDataLen;
//	all_len = ::htonl(all_len);
//
//	strOutData.clear();
//	strOutData.append((char*)&all_len, HEADER_LENTH);
//	strOutData.append((const char*)strData, unDataLen);
//
//	return (int)strOutData.size();
//	//return packetHandler_->enCode(strData, unDataLen, strOutData, msgid);
//}
//
//int Channel::deCode()
//{
//	if (!checkFlags(IChannel::FLAG_HANDSHAKE))
//	{
//		if (!handshake())
//		{
//			return false;
//		}
//	}
//
//	//packetHandler_->readHandler(readBuffer_);
//
//	//// 确保下次读取数据有足够的空间
//	//readBuffer_.normalize();
//	//readBuffer_.ensureFreeSpace();
//	return 0;
//}
//
//bool Channel::handshake()
//{
//	if (checkFlags(FLAG_HANDSHAKE))
//	{
//		return true;
//	}
//
//	setFlags(FLAG_HANDSHAKE, true);
//
//	std::string str((const char*)readBuffer_.getBasePointer(), readBuffer_.getActiveSize());
//	if (isWebSocketProtocol(str))
//	{
//		channelType_ = CHANNEL_WEBSOCKET;
//		//packetHandler_ = std::make_shared<WebSocketPacketHandler>(this);
//
//		// 回包表示握手成功
//		handShakeWebSocket(str);
//
//		readBuffer_.reset();
//
//		return false;
//	}
//
//	return true;
//}
//
//void Channel::recvCb(const char* data, size_t len, int msgid)
//{
//	inet_->handleMessage(this, data, len, msgid);
//
//	recvPakcetNum_++;
//}
//
//bool Channel::isWebSocketProtocol(const std::string& str)
//{
//	size_t fi = str.find("Sec-WebSocket-Key");
//	if (fi == std::string::npos)
//	{
//		return false;
//	}
//
//	fi = str.find("GET");
//	if (fi == std::string::npos)
//	{
//		return false;
//	}
//
//	std::vector<std::string> header_and_data;
//	header_and_data = util::zq_splits(str, "\r\n\r\n");
//	if (header_and_data.size() != 2)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool Channel::handShakeWebSocket(const std::string& str)
//{
//	std::vector<std::string> header_and_data;
//	header_and_data = util::zq_splits(str, "\r\n\r\n");
//	if (header_and_data.size() != 2)
//	{
//		return false;
//	}
//
//	std::unordered_map<std::string, std::string> headers;
//	std::vector<std::string> values;
//
//	values = util::zq_splits(header_and_data[0], "\r\n");
//	for (auto iter = values.begin(); iter != values.end(); ++iter)
//	{
//		std::string linedata = (*iter);
//		std::string::size_type findex = linedata.find_first_of(':', 0);
//		if (findex == std::string::npos)
//		{
//			continue;
//		}
//
//		std::string leftstr = linedata.substr(0, findex);
//		std::string rightstr = linedata.substr(findex + 1, linedata.size() - findex);
//
//		headers[util::zq_trim(leftstr)] = util::zq_trim(rightstr);
//	}
//
//	auto findIter = headers.find("Sec-WebSocket-Origin");
//	if (findIter == headers.end())
//	{
//		findIter = headers.find("Origin");
//		if (findIter == headers.end())
//		{
//			// 有些app级客户端可能没有这个字段
//			//return false;
//		}
//	}
//
//	std::string szOrigin;
//	if (findIter != headers.end())
//	{
//		szOrigin = fmt::format("WebSocket-Origin: {}\r\n", findIter->second);
//	}
//
//	findIter = headers.find("Sec-WebSocket-Key");
//	if (findIter == headers.end())
//	{
//		return false;
//	}
//
//	std::string szKey = findIter->second;
//
//	findIter = headers.find("Host");
//	if (findIter == headers.end())
//	{
//		return false;
//	}
//
//	std::string szHost = findIter->second;
//
//	// RFC6544_MAGIC_KEY
//	std::string server_key = szKey;
//	server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
//
//	SHA1 sha;
//	unsigned int message_digest[5];
//
//	sha.Reset();
//	sha << server_key.c_str();
//	sha.Result(message_digest);
//
//	for (int i = 0; i < 5; ++i)
//	{
//		message_digest[i] = ::htonl(message_digest[i]);
//	}
//
//	server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
//
//	std::string ackHandshake = fmt::format("HTTP/1.1 101 Switching Protocols\r\n"
//		"Upgrade: websocket\r\n"
//		"Connection: Upgrade\r\n"
//		"Sec-WebSocket-Accept: {}\r\n"
//		"{}"
//		"WebSocket-Location: ws://{}/WebManagerSocket\r\n"
//		"WebSocket-Protocol: WebManagerSocket\r\n\r\n",
//		server_key, szOrigin, szHost);
//
//	sendData(ackHandshake.c_str(), ackHandshake.size());
//
//	sendPakcetNum_++;
//
//	return true;
//}
//
//
