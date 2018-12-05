#include "game_socket.h"
#include "midware/game_logic/server/opcodes.h"
#include "midware/game_logic/server/world_packet.h"
#include "midware/game_logic/server/world_session.h"

#include "baselib/libloader/libmanager.h"

#include "interface_header/logic/IGameLogicModule.h"

namespace zq {


GameSocket::GameSocket(tcp_t::socket&& socket) :
	Socket(std::move(socket)), 
	_worldSession(nullptr)
{
	headerLengthBuffer_.resize(sizeof(ClientPktHeader));
}

void GameSocket::start()
{
	_worldSession = new WorldSession(shared_from_this());
	_worldSession->handleSendAuthSession();
}

void GameSocket::onClose()
{
	delete _worldSession;
	_worldSession = nullptr;
}

bool GameSocket::update()
{
    if (!BaseSocket::update())
        return false;

    return true;
}

void GameSocket::readHandler()
{
	if (!isOpen())
		return;

	MessageBuffer& packet = getReadBuffer();
	while (packet.getActiveSize() > 0)
	{
		if (!partialProcessPacket<GameSocket>(this, &GameSocket::readHeaderLengthHandler, headerLengthBuffer_, packet))
			break;

		if (!partialProcessPacket<GameSocket>(this, &GameSocket::readDataHandler, packetBuffer_, packet))
			break;

		headerLengthBuffer_.reset();
		packetBuffer_.reset();
	}

    asyncRead();
}

bool GameSocket::readHeaderLengthHandler()
{
	ASSERT(headerLengthBuffer_.getActiveSize() == sizeof(ClientPktHeader));

	_worldSession->getAuthCrypt().DecryptRecv(headerLengthBuffer_.getReadPointer(), sizeof(ClientPktHeader));

	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(headerLengthBuffer_.getReadPointer());
	EndianConvertReverse(header->size);
	EndianConvert(header->cmd);

	if (!header->IsValidSize() || !header->IsValidOpcode())
	{
		LOG_ERROR << fmt::format("WorldSocket::ReadHeaderHandler(): client %s sent malformed packet (size: %hu, cmd: %u)",
			getIp().c_str(), header->size, header->cmd);
		//return false;
	}

	header->size -= sizeof(header->cmd);
	packetBuffer_.resize(header->size);
	return true;
}

bool GameSocket::readDataHandler()
{
	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(headerLengthBuffer_.getReadPointer());
	OpcodeClient opcode = static_cast<OpcodeClient>(header->cmd);

	IGameLogicModule* game_logic = LibManager::get_instance().findModule<IGameLogicModule>();
	//game_logic

	WorldPacket packet(opcode, std::move(packetBuffer_));
	switch (opcode)
	{
	case CMSG_PING:
	case CMSG_AUTH_SESSION:
	case CMSG_KEEP_ALIVE: 

		game_logic->call(opcode, _worldSession, packet);
		break;
	default:
	{
		if (!_worldSession->isAuthed())
		{
			return false;
		}

		game_logic->call(opcode, _worldSession, packet);
	}
	}

	return true;
}

void GameSocket::sendPacket(MessageBuffer&& packet)
{
	queuePacket(std::move(packet));
}



}
