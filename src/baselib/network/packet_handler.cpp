//#include "packet_handler.h"
//using namespace zq;
//
//
//template<bool(PacketHandler::*processMethod)(), MessageBuffer PacketHandler::*outputBuffer>
//inline bool partialProcessPacket(PacketHandler* handler, MessageBuffer& inputBuffer)
//{
//	MessageBuffer& buffer = handler->*outputBuffer;
//
//	if (buffer.getRemainingSpace() > 0)
//	{
//		// readbuff�»��ж���û��ȡ���ֽڣ����buff����ʣ�µĶ��ٿռ�
//		std::size_t readDataSize = std::min(inputBuffer.getActiveSize(), buffer.getRemainingSpace());
//		buffer.write(inputBuffer.getReadPointer(), readDataSize);
//		inputBuffer.readCompleted(readDataSize);
//	}
//
//	if (buffer.getRemainingSpace() > 0)
//	{
//		// ����ʣ�µ��ֽ�û���գ��ȴ��´�����.
//		ASSERT(inputBuffer.getActiveSize() == 0);
//		return false;
//	}
//
//	// �����յ������buffer����Ҫ���ֽ���
//	if (!(handler->*processMethod)())
//	{
//		//session->CloseSocket();
//		return false;
//	}
//
//	return true;
//}
//
//PacketHandler::PacketHandler(FUN_DATA_CB&& cb)
//	: channel_(nullptr),
//	dataCb_(std::move(cb))
//{
//	headerLengthBuffer_.resize(HEADER_LENTH);
//	headerBuffer_.resize(0);
//	packetBuffer_.resize(0);
//}
//
//PacketHandler::~PacketHandler()
//{
//
//}
//
//int PacketHandler::enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid)
//{
//	// �ܳ���
//	uint32 all_len = (uint32)unDataLen;
//	all_len = ::htonl(all_len);
//
//	strOutData.clear();
//	strOutData.append((char*)&all_len, HEADER_LENTH);
//	strOutData.append(strData, unDataLen);
//
//	return (int)strOutData.size();
//}
//
//void PacketHandler::readHandler(IChannel* channel, MessageBuffer& packet)
//{
//	if (channel == nullptr)
//	{
//		return;
//	}
//
//	channel_ = channel;
//	while (packet.getActiveSize() > 0)
//	{
//		if (!partialProcessPacket<&PacketHandler::readHeaderLengthHandler, &PacketHandler::headerLengthBuffer_>(this, packet))
//			break;
//
//		if (!partialProcessPacket<&PacketHandler::readHeaderHandler, &PacketHandler::headerBuffer_>(this, packet))
//			break;
//
//		if (!partialProcessPacket<&PacketHandler::readDataHandler, &PacketHandler::packetBuffer_>(this, packet))
//			break;
//
//		headerLengthBuffer_.reset();
//		headerBuffer_.reset();
//		packetBuffer_.reset();
//	}
//}
//
//bool PacketHandler::readHeaderLengthHandler()
//{
//	uint32 data_len = ::ntohl(*(uint32*)headerLengthBuffer_.getBasePointer());
//	if (data_len > BUFFER_MAX_READ)
//	{
//		LOG_ERROR << "close channel, read headlenth error, datalen: " << data_len;
//		channel_->close();
//		return false;
//	}
//
//	packetBuffer_.resize(data_len);
//	return true;
//}
//
//bool PacketHandler::readHeaderHandler()
//{
//	return true;
//}
//
//bool PacketHandler::readDataHandler()
//{
//	dataCb_(channel_, (const char*)packetBuffer_.getBasePointer(), packetBuffer_.getBufferSize(), 0);
//	return true;
//}
//
//
//
//
//
//
///*--------------------------------------------------
//			ComponentPacketHandler
////-------------------------------------------------*/
//ComponentPacketHandler::ComponentPacketHandler(FUN_DATA_CB&& cb)
//	:PacketHandler(std::move(cb))
//{
//	headerLengthBuffer_.resize(HEADER_LENTH);
//	headerBuffer_.resize(0);
//	packetBuffer_.resize(0);
//}
//
//ComponentPacketHandler::~ComponentPacketHandler()
//{
//
//}
//
//int ComponentPacketHandler::enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid)
//{
//	// ͷ4���ֽڴ������ݳ���
//	uint32 all_len = (uint32)unDataLen;
//	all_len = ::htonl(all_len);
//
//	strOutData.clear();
//	strOutData.append((char*)&all_len, HEADER_LENTH);
//	strOutData.append(strData, unDataLen);
//
//	return (int)strOutData.size();
//}
//
//bool ComponentPacketHandler::readHeaderLengthHandler()
//{
//	uint32 data_len = ::ntohl(*(uint32*)headerLengthBuffer_.getBasePointer());
//	if (data_len > BUFFER_MAX_READ)
//	{
//		LOG_ERROR << "close channel, read headlenth error, datalen: " << data_len;
//		channel_->close();
//		return false;
//	}
//
//	packetBuffer_.resize(data_len);
//	return true;
//}
//
//bool ComponentPacketHandler::readHeaderHandler()
//{
//	return true;
//}
//
//bool ComponentPacketHandler::readDataHandler()
//{
//	dataCb_(channel_, (const char*)packetBuffer_.getBasePointer(), packetBuffer_.getBufferSize(), 0);
//	return true;
//}
//
//
//
//
//
//
//
//
//
//
//
///*--------------------------------------------------
//                 websocket
////-------------------------------------------------*/
//WebSocketPacketHandler::WebSocketPacketHandler(FUN_DATA_CB&& cb)
//	:PacketHandler(std::move(cb))
//{
//	headerLengthBuffer_.resize(0);
//	headerBuffer_.resize(EN_SHORT_HEADER);
//	packetBuffer_.resize(0);
//}
//
//WebSocketPacketHandler::~WebSocketPacketHandler()
//{
//
//}
//
//void WebSocketPacketHandler::reset()
//{
//	memset(&webSocketHead_, 0, sizeof(webSocketHead_));
//}
//
//int WebSocketPacketHandler::enCode(const char* strData, size_t unDataLen, std::string& strOutData, int msgid)
//{
//	return 0;
//}
//
//int WebSocketPacketHandler::getBit(char byte, int position)
//{
//	return ((byte & (1 << position)) >> position);
//}
//
//void WebSocketPacketHandler::setBit(char * buffer, int position)
//{
//	buffer[0] |= (1 << position);
//}
//
//int WebSocketPacketHandler::get16Bit(const char * buffer)
//{
//	int high_part = buffer[0] << 8;
//	int low_part = buffer[1] & 0x000000ff;
//
//	return (high_part | low_part) & 0x000000ffff;
//}
//void WebSocketPacketHandler::set16Bit(int value, char * buffer)
//{
//	buffer[0] = (value & 0x0000ff00) >> 8;
//	buffer[1] = value & 0x000000ff;
//}
//
//void WebSocketPacketHandler::decodeMaskData(char* pszData, int iDataLen, char* pMask)
//{
//	int iter = 0;
//	int mask_index = 0;
//
//	while (iter < iDataLen)
//	{
//		mask_index = iter % 4;
//		pszData[iter] ^= pMask[mask_index];
//		iter++;
//	}
//}
//
//bool WebSocketPacketHandler::parseHead(const char* data_buffer, uint32 buffer_len, uint32& left_size)
//{
//	// WebSocket֡���ݽṹ
//	/*
//	0                   1                   2                   3
//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	+-+-+-+-+-------+-+-------------+-------------------------------+
//	|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
//	|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
//	|N|V|V|V|       |S|             |   (if payload len==126/127)   |
//	| |1|2|3|       |K|             |                               |
//	+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
//	|     Extended payload length continued, if payload len == 127  |
//	+ - - - - - - - - - - - - - - - +-------------------------------+
//	|                               |Masking-key, if MASK set to 1  |
//	+-------------------------------+-------------------------------+
//	| Masking-key (continued)       |          Payload Data         |
//	+-------------------------------- - - - - - - - - - - - - - - - +
//	:                     Payload Data continued ...                :
//	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
//	|                     Payload Data continued ...                |
//	+---------------------------------------------------------------+
//	*/
//
//
//	if (buffer_len < EN_SHORT_HEADER)
//	{
//		return false;
//	}
//
//	memset(&webSocketHead_, 0, sizeof(webSocketHead_));
//
//	// ��һ���ֽ�, ���λ����������Ϣ�Ƿ����, ���4λ����������Ϣ����
//	// fin:1λ������������Ϣ�Ƿ���������Ϊ1�����ϢΪ��Ϣβ��,���Ϊ�����к������ݰ�;
//	webSocketHead_.has_fin = getBit(data_buffer[0], 7);
//
//	// opcode:�����4λ��ʾ������������Ϣ����
//	webSocketHead_.op_code = data_buffer[0] & 0x0F;
//
//	// �ڶ����ֽڣ���Ҫ���������������Ϣ����, ���λ��0��1�������Ƿ������봦��
//	// mask:1λ��������ʾ�Ƿ������룬1��ʾ���ݾ��������봦��
//	webSocketHead_.is_masked = getBit(data_buffer[1], 7);
//
//	// ʣ�µĺ���7λ����������Ϣ����, ����7λ���ֻ������127�������ֵ������������
//	// ���ݳ���С��126: ��ô��7λ�ͱ�ʾ������ݵ���ʵ����
//	// ����126: �����2���ֽ��γɵ�16λ�޷�����������ֵ��payload����ʵ����
//	// ����127: �����8���ֽ��γɵ�64λ�޷�����������ֵ��payload����ʵ����
//	webSocketHead_.payload_size = data_buffer[1] & 0x7F;
//
//	// �ͻ���֡��������������
//	if (!webSocketHead_.is_masked)
//	{
//		return false;
//	}
//
//	int pos = WS_HEAD_SIZE;
//	if (webSocketHead_.payload_size < 126)
//	{
//		left_size = 0;
//	}
//	else if (webSocketHead_.payload_size == 126)
//	{
//		webSocketHead_.payload_size = get16Bit(data_buffer + WS_HEAD_SIZE);
//		pos += WS_PAYLOAD_SIZE_UINT16;
//		left_size = EN_MEDIUM_HEADER - buffer_len;
//	}
//	else if (webSocketHead_.payload_size == 127)
//	{
//		unsigned char* len = (unsigned char*)data_buffer + WS_HEAD_SIZE;
//		webSocketHead_.payload_size = 0;
//		webSocketHead_.payload_size |= ((uint64)(len[0]) << 56);
//		webSocketHead_.payload_size |= ((uint64)(len[1]) << 48);
//		webSocketHead_.payload_size |= ((uint64)(len[2]) << 40);
//		webSocketHead_.payload_size |= ((uint64)(len[3]) << 32);
//		webSocketHead_.payload_size |= ((uint64)(len[4]) << 24);
//		webSocketHead_.payload_size |= ((uint64)(len[5]) << 16);
//		webSocketHead_.payload_size |= ((uint64)(len[6]) << 8);
//		webSocketHead_.payload_size |= len[7];
//
//		pos += WS_PAYLOAD_SIZE_UINT64;
//		left_size = EN_LONG_HEADER - buffer_len;
//	}
//	else
//	{
//		return false;
//	}
//
//	// ��ȡ4�ֽ�����ֵ
//	memcpy(webSocketHead_.mask, data_buffer + pos, WS_MASK_SIZE);
//
//	return true;
//}
//
//FrameType WebSocketPacketHandler::getFrameType()
//{
//	if (webSocketHead_.op_code == 0x0) return (webSocketHead_.has_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME; // continuation frame ?
//	if (webSocketHead_.op_code == 0x1) return (webSocketHead_.has_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
//	if (webSocketHead_.op_code == 0x2) return (webSocketHead_.has_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME;
//	if (webSocketHead_.op_code == 0x8) return CLOSE_FRAME;
//	if (webSocketHead_.op_code == 0x9) return PING_FRAME;
//	if (webSocketHead_.op_code == 0xA) return PONG_FRAME;
//	return ERROR_FRAME;
//}
//
//bool WebSocketPacketHandler::readHeaderLengthHandler()
//{
//	return true;
//}
//
//bool WebSocketPacketHandler::readHeaderHandler()
//{
//	uint32 left_size = 0;
//	bool ret = parseHead((const char*)headerBuffer_.getBasePointer(), (uint32)headerBuffer_.getBufferSize(), left_size);
//	if (!ret)
//	{
//		channel_->close();
//		return false;
//	}
//
//	if (left_size > 0)
//	{
//		headerBuffer_.resize(headerBuffer_.getBufferSize() + left_size);
//		return false;
//	}
//	else
//	{
//		packetBuffer_.resize(packetBuffer_.getBufferSize() + webSocketHead_.payload_size);
//	}
//
//	return true;
//}
//
//bool WebSocketPacketHandler::readDataHandler()
//{
//	FrameType frame_type = getFrameType();
//	switch (frame_type)
//	{
//	case ERROR_FRAME:
//	case CLOSE_FRAME:
//	{
//		channel_->close();
//		return false;
//	}
//	break;
//	case OPENING_FRAME:
//		break;
//	case TEXT_FRAME:
//	case BINARY_FRAME:
//	{
//		decodeMaskData((char*)packetBuffer_.getBasePointer(), (uint32)packetBuffer_.getBufferSize(), webSocketHead_.mask);
//
//		// ����ͻ�ȡ�����û����ݣ�Ȼ��������������
//		uint32 total_len = ::ntohl(*(uint32*)packetBuffer_.getBasePointer());
//		uint32 data_len = total_len - HEADER_LENTH;
//		const char* data = (const char*)packetBuffer_.getBasePointer() + HEADER_LENTH;
//		dataCb_(channel_, data, data_len, 0);
//	}
//	break;
//	case INCOMPLETE_BINARY_FRAME:
//	case INCOMPLETE_TEXT_FRAME:
//	{
//		// �ȴ���һ֡����
//		headerBuffer_.reset();
//		return false;
//	}
//	break;
//	case PING_FRAME:
//	{
//		wsPong();
//	}
//	break;
//	case PONG_FRAME:
//	{
//		wsPing();
//		break;
//	}
//	default:
//		break;
//	}
//
//	return true;
//}
//
//void WebSocketPacketHandler::wsPing()
//{
//
//}
//
//void WebSocketPacketHandler::wsPong()
//{
//
//}