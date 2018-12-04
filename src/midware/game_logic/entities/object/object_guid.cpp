#include "object_guid.h"
#include <sstream>
#include <iosfwd>
#include <iomanip>


template<typename T>
inline void hash_combine(std::size_t& seed, T const& val)
{
	seed ^= std::hash<T>()(val) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

//! Hash implementation for std::pair to allow using pairs in unordered_set or as key for unordered_map
//! Individual types used in pair must be hashable by std::hash
namespace std
{
	template<class K, class V>
	struct hash<std::pair<K, V>>
	{
	public:
		size_t operator()(std::pair<K, V> const& p) const
		{
			size_t hashVal = 0;
			hash_combine(hashVal, p.first);
			hash_combine(hashVal, p.second);
			return hashVal;
		}
	};
}

namespace zq{


ObjectGuid const ObjectGuid::Empty = ObjectGuid();

char const* ObjectGuid::GetTypeName(HighGuid high)
{
    switch (high)
    {
        case HighGuid::Item:         return "Item";
        case HighGuid::Player:       return "Player";
        case HighGuid::GameObject:   return "Gameobject";
        case HighGuid::Transport:    return "Transport";
        case HighGuid::Unit:         return "Creature";
        case HighGuid::Pet:          return "Pet";
        case HighGuid::Vehicle:      return "Vehicle";
        case HighGuid::DynamicObject: return "DynObject";
        case HighGuid::Corpse:       return "Corpse";
        case HighGuid::Mo_Transport: return "MoTransport";
        case HighGuid::Instance:     return "InstanceID";
        case HighGuid::Group:        return "Group";
        default:
            return "<unknown>";
    }
}

std::string ObjectGuid::ToString() const
{
    std::ostringstream str;
    str << "GUID Full: 0x" << std::hex << std::setw(16) << std::setfill('0') << _guid << std::dec;
    str << " Type: " << GetTypeName();
    if (HasEntry())
        str << (IsPet() ? " Pet number: " : " Entry: ") << GetEntry() << " ";

    str << " Low: " << GetCounter();
    return str.str();
}

ObjectGuid ObjectGuid::Global(HighGuid type, ObjectGuidLowType counter)
{
    return ObjectGuid(type, counter);
}

ObjectGuid ObjectGuid::MapSpecific(HighGuid type, uint32 entry, ObjectGuidLowType counter)
{
    return ObjectGuid(type, entry, counter);
}

ByteBuffer& operator<<(ByteBuffer& buf, ObjectGuid const& guid)
{
    buf << uint64(guid.GetRawValue());
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, ObjectGuid& guid)
{
    guid.Set(buf.read<uint64>());
    return buf;
}

ByteBuffer& operator<<(ByteBuffer& buf, PackedGuid const& guid)
{
    buf.append(guid._packedGuid);
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, PackedGuidReader const& guid)
{
    buf.readPackGUID(reinterpret_cast<uint64&>(guid.uuid));
    return buf;
}

void ObjectGuidGeneratorBase::HandleCounterOverflow(HighGuid high)
{
    //LOG_ERROR("%s guid overflow!! Can't continue, shutting down server. ", ObjectGuid::GetTypeName(high));
    //World::StopNow(ERROR_EXIT_CODE);
}

void ObjectGuidGeneratorBase::CheckGuidTrigger(ObjectGuidLowType guidlow)
{
    //if (!sWorld->IsGuidAlert() && guidlow > sWorld->getIntConfig(CONFIG_RESPAWN_GUIDALERTLEVEL))
    //    sWorld->TriggerGuidAlert();
    //else if (!sWorld->IsGuidWarning() && guidlow > sWorld->getIntConfig(CONFIG_RESPAWN_GUIDWARNLEVEL))
    //    sWorld->TriggerGuidWarning();
}


}