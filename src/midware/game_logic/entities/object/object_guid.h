#pragma once


#include "interface_header/base/platform.h"
#include "interface_header/logic/IObjectGuid.h"
#include "../../server/byte_buffer.hpp"
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>
#include <unordered_set>

namespace zq{


class ObjectGuid;
class PackedGuid;

struct PackedGuidReader
{
    explicit PackedGuidReader(ObjectGuid& guid) : uuid(guid) { }
    ObjectGuid& uuid;
};


// minimum buffer size for packed guid is 9 bytes
#define PACKED_GUID_MIN_BUFFER_SIZE 9

class PackedGuid
{
friend ByteBuffer& operator<<(ByteBuffer& buf, PackedGuid const& guid);

public:
    explicit PackedGuid() : _packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { _packedGuid.appendPackGUID(0); }
    explicit PackedGuid(uint64 guid) : _packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { _packedGuid.appendPackGUID(guid); }
    explicit PackedGuid(ObjectGuid guid) : _packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { _packedGuid.appendPackGUID(guid.GetRawValue()); }

    void Set(uint64 guid) { _packedGuid.wpos(0); _packedGuid.appendPackGUID(guid); }
    void Set(ObjectGuid guid) { _packedGuid.wpos(0); _packedGuid.appendPackGUID(guid.GetRawValue()); }

    std::size_t size() const { return _packedGuid.size(); }

private:
    ByteBuffer _packedGuid;
};

class ObjectGuidGeneratorBase
{
public:
    ObjectGuidGeneratorBase(ObjectGuidLowType start = 1) : _nextGuid(start) { }

    virtual void Set(ObjectGuidLowType val) { _nextGuid = val; }
    virtual ObjectGuidLowType Generate(HighGuid high) = 0;
    ObjectGuidLowType GetNextAfterMaxUsed() const { return _nextGuid; }
    virtual ~ObjectGuidGeneratorBase() { }

protected:
    static void HandleCounterOverflow(HighGuid high);
    static void CheckGuidTrigger(ObjectGuidLowType guid);
    ObjectGuidLowType _nextGuid;
};

class ObjectGuidGenerator : public ObjectGuidGeneratorBase
{
public:
    explicit ObjectGuidGenerator(ObjectGuidLowType start = 1) : ObjectGuidGeneratorBase(start) { }

    ObjectGuidLowType Generate(HighGuid high) override
    {
        if (_nextGuid >= ObjectGuid::GetMaxCounter(high) - 1)
            HandleCounterOverflow(high);

        if (high == HighGuid::Unit || high == HighGuid::GameObject)
            CheckGuidTrigger(_nextGuid);

        return _nextGuid++;
    }
};

ByteBuffer& operator<<(ByteBuffer& buf, ObjectGuid const& guid);
ByteBuffer& operator>>(ByteBuffer& buf, ObjectGuid&       guid);

ByteBuffer& operator<<(ByteBuffer& buf, PackedGuid const& guid);
ByteBuffer& operator>>(ByteBuffer& buf, PackedGuidReader const& guid);

}


namespace std
{
	template<>
	struct hash<zq::ObjectGuid>
	{
	public:
		size_t operator()(zq::ObjectGuid const& key) const
		{
			return std::hash<uint64_t>()(key.GetRawValue());
		}
	};
}
