#include "object.h"
#include "../player/player.h"

#include "baselib/message/game_db_object.pb.h"


namespace zq{

Object::Object() : m_PackGUID(sizeof(uint64)+1)
{
	m_objectTypeId = TYPEID_OBJECT;
	m_objectType = TYPEMASK_OBJECT;
}

Object::~Object()
{

}

void Object::initValue(PROERTY_TYPE type, size_t count)
{
	if (count <= 0)
		return;

	switch (type)
	{
	case PROERTY_TYPE::INT32:
		propertyInt32_.resize(count);
		std::fill(propertyInt32_.begin(), propertyInt32_.end(), 0);
		break;
	case PROERTY_TYPE::INT64:
		propertyInt64_.resize(count);
		std::fill(propertyInt64_.begin(), propertyInt64_.end(), 0);
		break;
	case PROERTY_TYPE::UINT64:
		propertyUint64_.resize(count);
		std::fill(propertyUint64_.begin(), propertyUint64_.end(), 0);
		break;
	case PROERTY_TYPE::FLOAT:
		propertyFloat_.resize(count);
		std::fill(propertyFloat_.begin(), propertyFloat_.end(), 0.f);
		break;
	case PROERTY_TYPE::DOUBLE:
		propertyDouble_.resize(count);
		std::fill(propertyDouble_.begin(), propertyDouble_.end(), 0.0);
		break;
	case PROERTY_TYPE::STRING:
		propertyString_.resize(count);
		break;
	default:
		break;
	}
}

void Object::setValueInt32(size_t index, int32 value)
{
	ASSERT(index < propertyInt32_.size());
	propertyInt32_[index] = value;
}

void Object::setValueInt64(size_t index, int64 value)
{
	ASSERT(index < propertyInt64_.size());
	propertyInt64_[index] = value;
}

void Object::setValueUint64(size_t index, uint64 value)
{
	ASSERT(index < propertyUint64_.size());
	propertyUint64_[index] = value;
}

void Object::setValueFloat(size_t index, float value)
{
	ASSERT(index < propertyFloat_.size());
	propertyFloat_[index] = value;
}

void Object::setValueDouble(size_t index, double value)
{
	ASSERT(index < propertyDouble_.size());
	propertyDouble_[index] = value;
}

void Object::setValueString(size_t index, const std::string& value)
{
	ASSERT(index < propertyString_.size());
	propertyString_[index] = value;
}

void Object::setValueGuid(size_t index, const ObjectGuid& value)
{
	ASSERT(index < propertyUint64_.size());
	propertyUint64_[index] = value;
}

int32 Object::getValueInt32(size_t index)
{
	ASSERT(index < propertyInt32_.size());
	return propertyInt32_[index];
}

int64 Object::getValueInt64(size_t index)
{
	ASSERT(index < propertyInt64_.size());
	return propertyInt64_[index];
}

uint64 Object::getValueUint64(size_t index)
{
	ASSERT(index < propertyUint64_.size());
	return propertyUint64_[index];
}

float Object::getValueFloat(size_t index)
{
	ASSERT(index < propertyFloat_.size());
	return propertyFloat_[index];
}

double Object::getValueDouble(size_t index)
{
	ASSERT(index < propertyDouble_.size());
	return propertyDouble_[index];
}

const std::string& Object::getValueString(size_t index)
{
	ASSERT(index < propertyString_.size());
	return propertyString_[index];
}

ObjectGuid Object::getValueGuid(size_t index)
{
	ASSERT(index < propertyUint64_.size());
	return ObjectGuid(propertyUint64_[index]);
}

void Object::loadFromDb(DBObject::DBObjectInfo& db_object)
{
	// int32
	const DBObject::DBPropertyInt32& prop_int32 = db_object.property_int32();
	for (const auto& ele : prop_int32.map_int32())
	{
		int32 index = ele.first;
		if (propertyInt32_.size() <= index)
		{
			LOG_ERROR << fmt::format("int32 index invalid, propersize:{},index:{}.", propertyInt32_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyInt32_[index] = ele.second;
	}

	// int64
	const DBObject::DBPropertyInt64& prop_int64 = db_object.property_int64();
	for (const auto& ele : prop_int64.map_int64())
	{
		int32 index = ele.first;
		if (propertyInt64_.size() <= index)
		{
			LOG_ERROR << fmt::format("int64 index invalid, propersize:{},index:{}.", propertyInt64_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyInt64_[index] = ele.second;
	}

	// uint64
	const DBObject::DBPropertyUint64& prop_uint64 = db_object.property_uint64();
	for (const auto& ele : prop_uint64.map_uint64())
	{
		int32 index = ele.first;
		if (propertyUint64_.size() <= index)
		{
			LOG_ERROR << fmt::format("uint64 index invalid, propersize:{},index:{}.", propertyUint64_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyUint64_[index] = ele.second;
	}

	// float
	const DBObject::DBPropertyFloat& prop_float = db_object.property_float();
	for (const auto& ele : prop_float.map_float())
	{
		int32 index = ele.first;
		if (propertyFloat_.size() <= index)
		{
			LOG_ERROR << fmt::format("float index invalid, propersize:{},index:{}.", propertyFloat_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyFloat_[index] = ele.second;
	}

	// uint64
	const DBObject::DBPropertyDouble& prop_double = db_object.property_double();
	for (const auto& ele : prop_double.map_double())
	{
		int32 index = ele.first;
		if (propertyDouble_.size() <= index)
		{
			LOG_ERROR << fmt::format("double index invalid, propersize:{},index:{}.", propertyDouble_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyDouble_[index] = ele.second;
	}

	// string
	const DBObject::DBPropertyString& prop_string = db_object.property_string();
	for (const auto& ele : prop_string.map_string())
	{
		int32 index = ele.first;
		if (propertyString_.size() <= index)
		{
			LOG_ERROR << fmt::format("string index invalid, propersize:{},index:{}.", propertyString_.size(), index);
			ASSERT(false);
			continue;
		}

		propertyString_[index] = ele.second;
	}
}

void Object::saveDb(DBObject::DBObjectInfo& db_object)
{
	// int32
	DBObject::DBPropertyInt32* prop_int32 = db_object.mutable_property_int32();
	for (int32 i = 0; i < (int32)propertyInt32_.size(); ++i)
	{
		prop_int32->mutable_map_int32()->insert({i, propertyInt32_ [i]});
	}
	
	// int64
	DBObject::DBPropertyInt64* prop_int64 = db_object.mutable_property_int64();
	for (int32 i = 0; i < (int32)propertyInt64_.size(); ++i)
	{
		prop_int64->mutable_map_int64()->insert({ i, propertyInt64_[i] });
	}

	// uint64
	DBObject::DBPropertyUint64* prop_uint64 = db_object.mutable_property_uint64();
	for (int32 i = 0; i < (int32)propertyUint64_.size(); ++i)
	{
		prop_uint64->mutable_map_uint64()->insert({ i, propertyUint64_[i] });
	}

	// float
	DBObject::DBPropertyFloat* prop_float = db_object.mutable_property_float();
	for (int32 i = 0; i < (int32)propertyFloat_.size(); ++i)
	{
		prop_float->mutable_map_float()->insert({ i, propertyFloat_[i] });
	}

	// double
	DBObject::DBPropertyDouble* prop_double = db_object.mutable_property_double();
	for (int32 i = 0; i < (int32)propertyDouble_.size(); ++i)
	{
		prop_double->mutable_map_double()->insert({ i, propertyDouble_[i] });
	}

	// string
	DBObject::DBPropertyString* prop_string = db_object.mutable_property_string();
	for (int32 i = 0; i < (int32)propertyString_.size(); ++i)
	{
		prop_string->mutable_map_string()->insert({ i, propertyString_[i] });
	}
}

void Object::savePropertyInt32()
{

}

void Object::savePropertyInt64()
{

}

void Object::savePropertyUint64()
{

}

void Object::savePropertyString()
{

}


}
