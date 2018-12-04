#pragma once


#include "object_guid.h"
#include "object_defines.h"
#include <list>
#include <set>
#include <unordered_map>

namespace DBObject
{
	class DBObjectInfo;
}

namespace zq{


class Object
{
protected:

	enum class PROERTY_TYPE
	{
		INT32, INT64, UINT64,
		FLOAT, DOUBLE, STRING, UUID,
	};

public:

	Object();
	virtual ~Object();

	void initValue(PROERTY_TYPE type, size_t count);
	void setValueInt32(size_t index, int32 value);
	void setValueInt64(size_t index, int64 value);
	void setValueUint64(size_t index, uint64 value);
	void setValueFloat(size_t index, float value);
	void setValueDouble(size_t index, double value);
	void setValueString(size_t index, const std::string& value);
	void setValueGuid(size_t index, const ObjectGuid& value);

	virtual void loadFromDb(DBObject::DBObjectInfo& db_object);
	virtual void saveDb(DBObject::DBObjectInfo& db_object);

protected:

	virtual void savePropertyInt32();
	virtual void savePropertyInt64();
	virtual void savePropertyUint64();
	virtual void savePropertyString();

protected:

	uint16 m_objectType;
	TypeID m_objectTypeId;

private:

	std::vector<int32> propertyInt32_;
	std::vector<int64> propertyInt64_;
	std::vector<uint64> propertyUint64_;
	std::vector<float> propertyFloat_;
	std::vector<double> propertyDouble_;
	std::vector<std::string> propertyString_;

    PackedGuid m_PackGUID;

    Object(Object const& right) = delete;
    Object& operator=(Object const& right) = delete;
};


}
