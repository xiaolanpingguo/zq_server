#pragma once


#include "world_packet.h"

namespace zq{
namespace WorldPackets{

class Packet
{
public:
    Packet(WorldPacket&& worldPacket);

    virtual ~Packet() = default;

    Packet(Packet const& right) = delete;
    Packet& operator=(Packet const& right) = delete;

    virtual WorldPacket const* Write() = 0;
    virtual void Read() = 0;

    WorldPacket const* GetRawPacket() const { return &_worldPacket; }
    size_t GetSize() const { return _worldPacket.size(); }

protected:
    WorldPacket _worldPacket;
};

class ServerPacket : public Packet
{
public:
    ServerPacket(OpcodeServer opcode, size_t initialSize = 200);

    void Read() override final;

    void Clear() { _worldPacket.clear(); }
    WorldPacket&& Move() { return std::move(_worldPacket); }
    void ShrinkToFit() { _worldPacket.shrink_to_fit(); }

    OpcodeServer GetOpcode() const { return OpcodeServer(_worldPacket.GetOpcode()); }
};

class ClientPacket : public Packet
{
public:
    ClientPacket(WorldPacket&& packet);
    ClientPacket(OpcodeClient expectedOpcode, WorldPacket&& packet);

    WorldPacket const* Write() override final;

    OpcodeClient GetOpcode() const { return OpcodeClient(_worldPacket.GetOpcode()); }
};
}

}
