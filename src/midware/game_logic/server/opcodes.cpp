#include "opcodes.h"
#include "world_packet.h"
#include "world_session.h"

#include <iomanip>
#include <sstream>

namespace zq{


template<class PacketClass, void(WorldSession::*HandlerFunction)(PacketClass&)>
class PacketHandler : public ClientOpcodeHandler
{
public:
    PacketHandler(char const* name, SessionStatus status, PacketProcessing processing) : ClientOpcodeHandler(name, status, processing) { }

    void Call(WorldSession* session, WorldPacket& packet) const override
    {
        PacketClass nicePacket(std::move(packet));
        nicePacket.Read();
        (session->*HandlerFunction)(nicePacket);
    }
};

template<void(WorldSession::*HandlerFunction)(WorldPacket&)>
class PacketHandler<WorldPacket, HandlerFunction> : public ClientOpcodeHandler
{
public:
    PacketHandler(char const* name, SessionStatus status, PacketProcessing processing) : ClientOpcodeHandler(name, status, processing) { }

    void Call(WorldSession* session, WorldPacket& packet) const override
    {
        (session->*HandlerFunction)(packet);
    }
};

OpcodeTable opcodeTable;

template<typename T>
struct get_packet_class
{
};

template<typename PacketClass>
struct get_packet_class<void(WorldSession::*)(PacketClass&)>
{
    using type = PacketClass;
};

OpcodeTable::OpcodeTable()
{
    memset(_internalTableClient, 0, sizeof(_internalTableClient));
}

OpcodeTable::~OpcodeTable()
{
    for (uint16 i = 0; i < NUM_OPCODE_HANDLERS; ++i)
        delete _internalTableClient[i];
}

template<typename Handler, Handler HandlerFunction>
void OpcodeTable::ValidateAndSetClientOpcode(OpcodeClient opcode, char const* name, SessionStatus status, PacketProcessing processing)
{
    if (uint32(opcode) == NULL_OPCODE)
    {
        LOG_ERROR << "Opcode: " << name << "does not have a value";
        return;
    }

    if (uint32(opcode) >= NUM_OPCODE_HANDLERS)
    {
		LOG_ERROR << "Tried to set handler for an invalid opcode: " << opcode;
        return;
    }

    if (_internalTableClient[opcode] != nullptr)
    {
		LOG_ERROR << "Tried to override client handler of " << opcodeTable[opcode]->Name << name << " with"  << opcode;
        return;
    }

    _internalTableClient[opcode] = new PacketHandler<typename get_packet_class<Handler>::type, HandlerFunction>(name, status, processing);
}

void OpcodeTable::ValidateAndSetServerOpcode(OpcodeServer opcode, char const* name, SessionStatus status)
{
    if (uint32(opcode) == NULL_OPCODE)
    {
		LOG_ERROR << "Opcode: " << name << "does not have a value";
        return;
    }

    if (uint32(opcode) >= NUM_OPCODE_HANDLERS)
    {
		LOG_ERROR << "Tried to set handler for an invalid opcode: " << opcode;
        return;
    }

    if (_internalTableClient[opcode] != nullptr)
    {
		LOG_ERROR << "Tried to override client handler of " << opcodeTable[opcode]->Name << name << " with" << opcode;
        return;
    }

    //_internalTableClient[opcode] = new PacketHandler<WorldPacket, &WorldSession::Handle_ServerSide>(name, status, PROCESS_INPLACE);
}

/// Correspondence between opcodes and their names
void OpcodeTable::Initialize()
{
#define DEFINE_HANDLER(opcode, status, processing, handler) \
    ValidateAndSetClientOpcode<decltype(handler), handler>(opcode, #opcode, status, processing)

#define DEFINE_SERVER_OPCODE_HANDLER(opcode, status) \
    static_assert(status == STATUS_NEVER || status == STATUS_UNHANDLED, "Invalid status for server opcode"); \
    ValidateAndSetServerOpcode(opcode, #opcode, status)


#undef DEFINE_HANDLER

#undef DEFINE_SERVER_OPCODE_HANDLER
}

template<typename T>
inline std::string GetOpcodeNameForLoggingImpl(T id)
{
    uint16 opcode = uint16(id);
    std::ostringstream ss;
    ss << '[';

    if (static_cast<uint16>(id) < NUM_OPCODE_HANDLERS)
    {
        if (OpcodeHandler const* handler = opcodeTable[id])
            ss << handler->Name;
        else
            ss << "UNKNOWN OPCODE";
    }
    else
        ss << "INVALID OPCODE";

    ss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << opcode << std::nouppercase << std::dec << " (" << opcode << ")]";
    return ss.str();
}

std::string GetOpcodeNameForLogging(Opcodes opcode)
{
    return GetOpcodeNameForLoggingImpl(opcode);
}

}
