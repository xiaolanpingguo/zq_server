#pragma once


#include "property_manager.h"
#include "IObject.h"

namespace zq {

class CObject : public IObject
{
	using CharList = std::vector<int8>;
	using Int32List = std::vector<int32>;
	using Uint32List = std::vector<uint32>;
	using Int64List = std::vector<int64>;
	using Uint64List = std::vector<uint64>;
	using FloatList = std::vector<float>;
	using DoubleList = std::vector<double>;
	using StringList = std::vector<std::string>;
	using UUIDList = std::vector<uuid>;
public:	  

	CObject(uuid id = NULL_UUID, const std::string& name = "");
	virtual ~CObject();

	bool exsitProperty(const std::string& name) override;
	IPropertyPtr getProperty(const std::string& name) override;
	bool addProperty(IPropertyPtr prop) override;

	IPropertyPtr setValueChar(const std::string& name, int8 value);
	IPropertyPtr setValueInt32(const std::string& name, int32 value);
	IPropertyPtr setValueUint32(const std::string& name, uint32 value);
	IPropertyPtr setValueInt64(const std::string& name, int64 value);
	IPropertyPtr setValueUint64(const std::string& name, uint64 value);
	IPropertyPtr setValueFloat(const std::string& name, float value);
	IPropertyPtr setValueDouble(const std::string& name, double value);
	IPropertyPtr setValueStr(const std::string& name, const std::string& value);
	IPropertyPtr setValueUUID(const std::string& name, const uuid& value);

	int8 getValueChar(const std::string& name);
	int32 getValueInt32(const std::string& name);
	uint32 getValueUint32(const std::string& name);
	int64 getValueInt64(const std::string& name);
	uint64 getValueUint64(const std::string& name);
	float getValueFloat(const std::string& name);
	double getValueDouble(const std::string& name);
	const std::string& getValueStr(const std::string& name);
	const uuid& getValueUUID(const std::string& name);


	bool exsitArrayProperty(const std::string& name) ;
	PropertyArrayPtr getArrayProperty(const std::string& name) ;
	bool appenArrayProperty(PropertyArrayPtr prop) ;
	IPropertyPtr getPropertyPos(const std::string& name, size_t pos) ;

	PropertyArrayPtr appendValueChar(const std::string& name, const CharList& value);
	PropertyArrayPtr appendValueInt32(const std::string& name, const Int32List& value);
	PropertyArrayPtr appendValueUint32(const std::string& name, const Uint32List& value);
	PropertyArrayPtr appendValueInt64(const std::string& name, const Int64List& valuee);
	PropertyArrayPtr appendValueUint64(const std::string& name, const Uint64List& value);
	PropertyArrayPtr appendValueFloat(const std::string& name, const FloatList& value);
	PropertyArrayPtr appendValueDouble(const std::string& name, const DoubleList& value);
	PropertyArrayPtr appendValueStr(const std::string& name, const StringList& value);
	PropertyArrayPtr appendValueUUID(const std::string& name, const UUIDList& value);

	IPropertyPtr setArrayValueChar(const std::string& name, const int8& value, size_t pos);
	IPropertyPtr setArrayValueInt32(const std::string& name, const int32& value, size_t pos);
	IPropertyPtr setArrayValueUint32(const std::string& name, const uint32& value, size_t pos);
	IPropertyPtr setArrayValueInt64(const std::string& name, const int64& valuee, size_t pos);
	IPropertyPtr setArrayValueUint64(const std::string& name, const uint64& value, size_t pos);
	IPropertyPtr setArrayValueFloat(const std::string& name, const float& value, size_t pos);
	IPropertyPtr setArrayValueDouble(const std::string& name, const double& value, size_t pos);
	IPropertyPtr setArrayValueStr(const std::string& name, const std::string& value, size_t pos);
	IPropertyPtr setArrayValueUUID(const std::string& name, const uuid& value, size_t pos);

	int8 getArrayValueChar(const std::string& name, size_t pos) { return getValuePos<int8>(name, pos); }
	int32 getArrayValueInt32(const std::string& name, size_t pos);
	uint32 getArrayValueUint32(const std::string& name, size_t pos);
	int64 getArrayValueInt64(const std::string& name, size_t pos);
	uint64 getArrayValueUint64(const std::string& name, size_t pos);
	float getArrayValueFloat(const std::string& name, size_t pos);
	double getArrayValueDouble(const std::string& name, size_t pos);
	const std::string& getArrayValueStr(const std::string& name, size_t pos);
	const uuid& getArrayValueUUID(const std::string& name, size_t pos);

	void setObjName(const std::string& v) { name_ = v; }
	const std::string& getObjName() override { return name_; }

	void setClassName(const std::string& v) { name_ = v; }
	const std::string& getClassName() override { return name_; }

	const uuid& getGuid() override { return self_; }

protected:

	template <typename T>
	std::shared_ptr<Property<T>> newProperty(const std::string& name, const T& value)
	{
		return std::make_shared<Property<T>>(name, value);
	}

	template <typename T>
	constexpr std::pair<const T&, VR_DATA_TYPE> type2Value()
	{
		if constexpr (std::is_same_v<T, int8>) return { NULL_CHAR, VR_DATA_TYPE::CHAR };
		else if constexpr (std::is_same_v<T, int32>) return { NULL_INT32, VR_DATA_TYPE::INT32 };
		else if constexpr (std::is_same_v<T, uint32>) return { NULL_UINT32, VR_DATA_TYPE::UINT32 };
		else if constexpr (std::is_same_v<T, int64>) return { NULL_INT64, VR_DATA_TYPE::INT64 };
		else if constexpr (std::is_same_v<T, uint64>) return { NULL_UINT64, VR_DATA_TYPE::UINT64 };
		else if constexpr (std::is_same_v<T, float>) return { NULL_FLOAT, VR_DATA_TYPE::FLOAT };
		else if constexpr (std::is_same_v<T, double>) return { NULL_DOUBLE, VR_DATA_TYPE::DOUBLE };
		else if constexpr (std::is_same_v<T, std::string>) return { NULL_STRING, VR_DATA_TYPE::STRING };
		else if constexpr (std::is_same_v<T, uuid>) return { NULL_UUID, VR_DATA_TYPE::UUID };
		else  static_assert(false, "property unsupport type!");
	}

	template<typename T>
	IPropertyPtr setValue(const std::string& name, const T& value)
	{
		IPropertyPtr prop = getProperty(name);
		if (prop == nullptr)
		{
			auto new_prop = std::make_shared<Property<T>>(name, value);
			new_prop->setValue(value);
			propertys_[prop->getName()] = prop;
			return new_prop;
		}
		else
		{
			std::static_pointer_cast<Property<T>>(prop)->setValue(value);
			return prop;
		}
	}

	template<typename T>
	const T& getValue(const std::string& name)
	{
		IPropertyPtr prop = getProperty(name);
		if (prop == nullptr)
		{
			return type2Value<T>().first;
		}

		if (prop->getType() != type2Value<T>().second)
		{
			return type2Value<T>().first;
		}

		return std::static_pointer_cast<Property<T>>(prop)->getValue();
	}

	template <typename T>
	PropertyArrayPtr appendValue(const std::string& name, const T& value)
	{
		auto prop_array = getArrayProperty(name);
		if (prop_array == nullptr)
		{
			prop_array = std::make_shared<PropertyArray>(name);
		}

		for (const auto& ele : value)
		{
			prop_array->appenProperty(newProperty(name, ele));
		}

		return prop_array;
	}

	template <typename T>
	IPropertyPtr setValuePos(const std::string& name, const T& value, size_t pos)
	{
		PropertyArrayPtr prop_array = getArrayProperty(name);
		if (prop_array == nullptr)
		{
			return nullptr;
		}

		if (prop_array->getSize() <= pos)
		{
			return nullptr;
		}

		if constexpr (std::is_same_v<T, int8>)
		{
			std::static_pointer_cast<Property<int8>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			std::static_pointer_cast<Property<int32>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, uint32>)
		{
			std::static_pointer_cast<Property<uint32>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			std::static_pointer_cast<Property<int64>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, uint64>)
		{
			std::static_pointer_cast<Property<uint64>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			std::static_pointer_cast<Property<float>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			std::static_pointer_cast<Property<double>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			std::static_pointer_cast<Property<std::string>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else if constexpr (std::is_same_v<T, uuid>)
		{
			std::static_pointer_cast<Property<uuid>>((*prop_array)[pos])->setValue(value);
			return (*prop_array)[pos];
		}
		else
		{
			static_assert(false, "property unsupport type!");
			return nullptr;
		}
	}

	template <typename T>
	const T& getValuePos(const std::string& name, size_t pos)
	{
		PropertyArrayPtr prop_array = getArrayProperty(name);
		if (prop_array == nullptr)
		{
			return type2Value<T>().first;
		}

		if (prop_array->getSize() <= pos)
		{
			return type2Value<T>().first;
		}

		auto prop = (*prop_array)[pos];
		if (prop->getType() != type2Value<T>().second)
		{
			return type2Value<T>().first;
		}

		return std::static_pointer_cast<Property<T>>(prop)->getValue();
	}

private:

	uuid self_;
	std::string name_;

	std::unordered_map<std::string, IPropertyPtr> propertys_;   // 基本类型
	std::unordered_map<std::string, PropertyArrayPtr> propertyArray_;  // 数组类型
};

}
