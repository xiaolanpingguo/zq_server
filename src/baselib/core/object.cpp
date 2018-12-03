#include "object.h"


namespace zq{

CObject::CObject(uuid id, const std::string& name)
	:self_(id), name_(name)
{

}

CObject::~CObject()
{

}

bool CObject::exsitProperty(const std::string& strPropertyName)
{
	return propertys_.find(strPropertyName) != propertys_.end();
}

IPropertyPtr CObject::getProperty(const std::string& strPropertyName)
{
	auto it = propertys_.find(strPropertyName);
	if (it == propertys_.end())
	{
		return nullptr;
	}

	return it->second;
}

bool CObject::addProperty(IPropertyPtr prop)
{
	if (prop == nullptr)
	{
		return false;
	}

	auto it = propertys_.find(prop->getName());
	if (it != propertys_.end())
	{
		return false;
	}

	propertys_[prop->getName()] = prop;
	return true;
}

IPropertyPtr CObject::setValueChar(const std::string& name, int8 value)
{
	return setValue<int8>(name, value);
}

IPropertyPtr CObject::setValueInt32(const std::string& name, int32 value)
{
	return setValue<int32>(name, value);
}

IPropertyPtr CObject::setValueUint32(const std::string& name, uint32 value)
{
	return setValue<uint32>(name, value);
}

IPropertyPtr CObject::setValueInt64(const std::string& name, int64 value)
{
	return setValue<int64>(name, value);
}

IPropertyPtr CObject::setValueUint64(const std::string& name, uint64 value)
{
	return setValue<uint64>(name, value);
}

IPropertyPtr CObject::setValueFloat(const std::string& name, float value)
{
	return setValue<float>(name, value);
}

IPropertyPtr CObject::setValueDouble(const std::string& name, double value)
{
	return setValue<double>(name, value);
}

IPropertyPtr CObject::setValueStr(const std::string& name, const std::string& value)
{
	return setValue<std::string>(name, value);
}

IPropertyPtr CObject::setValueUUID(const std::string& name, const uuid& value)
{
	return setValue<uuid>(name, value);
}

int8 CObject::getValueChar(const std::string& name)
{
	return getValue<int8>(name);
}

int32 CObject::getValueInt32(const std::string& name)
{
	return getValue<int32>(name);
}

uint32 CObject::getValueUint32(const std::string& name)
{
	return getValue<uint32>(name);
}

int64 CObject::getValueInt64(const std::string& name)
{;
	return getValue<int64>(name);
}

uint64 CObject::getValueUint64(const std::string& name)
{
	return getValue<uint64>(name);
}

float CObject::getValueFloat(const std::string& name)
{
	return getValue<float>(name);
}

double CObject::getValueDouble(const std::string& name)
{
	return getValue<double>(name);
}

const std::string& CObject::getValueStr(const std::string& name)
{
	return getValue<std::string>(name);
}

const uuid& CObject::getValueUUID(const std::string& name)
{
	return getValue<uuid>(name);
}

bool CObject::exsitArrayProperty(const std::string& strPropertyName)
{
	return propertyArray_.find(strPropertyName) != propertyArray_.end();
}

PropertyArrayPtr CObject::getArrayProperty(const std::string& strPropertyName)
{
	auto it = propertyArray_.find(strPropertyName);
	if (it == propertyArray_.end())
	{
		return nullptr;
	}

	return it->second;
}

bool CObject::appenArrayProperty(PropertyArrayPtr prop)
{
	if (prop == nullptr)
	{
		return false;
	}

	auto it = propertyArray_.find(prop->getName());
	if (it != propertyArray_.end())
	{
		return false;
	}

	propertyArray_[prop->getName()] = prop;
	return true;
}

IPropertyPtr CObject::getPropertyPos(const std::string& name, size_t pos)
{
	/*PropertyArrayPtr prop_array = getArrayProperty(name);
	if (prop_array == nullptr)
	{
		return nullptr;
	}

	if (prop_array->getSize() <= pos)
	{
		return nullptr;
	}

	auto prop = std::static_pointer_cast<(*prop_array)[pos]>;
	if (prop->getType() != VR_DATA_TYPE::CHAR)
	{
		return NULL_CHAR;
	}

	return prop->getValue();*/
	return nullptr;
}

PropertyArrayPtr CObject::appendValueChar(const std::string& name, const CharList& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueInt32(const std::string& name, const Int32List& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueUint32(const std::string& name, const Uint32List& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueInt64(const std::string& name, const Int64List& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueUint64(const std::string& name, const Uint64List& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueFloat(const std::string& name, const FloatList& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueDouble(const std::string& name, const DoubleList& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueStr(const std::string& name, const StringList& value)
{
	return appendValue(name, value);
}

PropertyArrayPtr CObject::appendValueUUID(const std::string& name, const UUIDList& value)
{
	return appendValue(name, value);
}

IPropertyPtr CObject::setArrayValueChar(const std::string& name, const int8& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueInt32(const std::string& name, const int32& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueUint32(const std::string& name, const uint32& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueInt64(const std::string& name, const int64& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueUint64(const std::string& name, const uint64& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueFloat(const std::string& name, const float& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueDouble(const std::string& name, const double& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueStr(const std::string& name, const std::string& value, size_t pos)
{
	return setValuePos(name, value, pos);
}

IPropertyPtr CObject::setArrayValueUUID(const std::string& name, const uuid& value, size_t pos)
{
	return setValuePos(name, value, pos);
}


}