#pragma once


#include <list>
#include <variant>
#include "interface_header/base/platform.h"
#include "interface_header/base/uuid.h"

namespace zq {



enum class VR_DATA_TYPE
{
	CHAR, INT32, UINT32, INT64, UINT64,
	FLOAT, DOUBLE, STRING, UUID,
};


static const int8 NULL_CHAR = std::numeric_limits<int8>::min();
static const int32 NULL_INT32 = 0;
static const uint32 NULL_UINT32 = 0;
static const int64 NULL_INT64 = 0;
static const uint64 NULL_UINT64 = 0;
static const float NULL_FLOAT = 0.0f;
static const double NULL_DOUBLE = 0.0;
static const std::string NULL_STRING = "";
static const uuid NULL_UUID = uuid();

class IProperty;
class PropertyArray;
using IPropertyPtr = std::shared_ptr<IProperty>;
using PropertyArrayPtr = std::shared_ptr<PropertyArray>;
class IProperty
{
public:

	VR_DATA_TYPE getType()const { return type_; }
	const std::string& getName() const { return name_; }
protected:

	template <typename T>
	struct EventHandler
	{
		using EventFun = std::function<void(const std::string&, const T&, const T&)>;
		using VecFun = std::vector<EventFun>;
		void registerCallback(EventFun&& cb)
		{
			propertyCb_.emplace_back(std::move(cb));
		}

		void onEventHandler(const std::string& name, const T& oldVar, const T& newVar)
		{
			for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
			{
				(*it)(name, oldVar, newVar);
			}
		}

		VecFun propertyCb_;
	};

	IProperty(std::string_view name, VR_DATA_TYPE type) : name_(name), type_(type) {}
	virtual ~IProperty() = default;

	VR_DATA_TYPE type_;
	std::string name_;
	std::variant<int8, int32, uint32, int64, uint64, float, double, std::string, uuid> data_;
};


template<typename T> struct Property;

template<> struct Property<int8> : public IProperty
{
	Property(std::string_view name, int8 v) : IProperty(name, VR_DATA_TYPE::CHAR) { data_ = v; }
	const int8& getValue() { return std::get<int8>(data_); }

	void setValue(int8 v)
	{
		int8 old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<int8>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<int8> eventHandler_;
};


template<> struct Property<int32> : public IProperty
{
	Property(std::string_view name, int32 v) : IProperty(name, VR_DATA_TYPE::INT32) { data_ = v; }
	const int32& getValue() { return std::get<int32>(data_); }

	void setValue(int32 v)
	{
		int32 old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<int32>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<int32> eventHandler_;
};

template<> struct Property<uint32> : public IProperty
{
	Property(std::string_view name, uint32 v) : IProperty(name, VR_DATA_TYPE::UINT32) { data_ = v; }
	const uint32& getValue() { return std::get<uint32>(data_); }

	void setValue(uint32 v)
	{
		uint32 old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<uint32>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<uint32> eventHandler_;
};

template<> struct Property<int64> : public IProperty
{
	Property(std::string_view name, int64 v) : IProperty(name, VR_DATA_TYPE::INT64) { data_ = v; }
	const int64& getValue() { return std::get<int64>(data_); }

	void setValue(int64 v)
	{
		int64 old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<int64>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<int64> eventHandler_;
};

template<> struct Property<uint64> : public IProperty
{
	Property(std::string_view name, uint64 v) : IProperty(name, VR_DATA_TYPE::UINT64) { data_ = v; }
	const uint64& getValue() { return std::get<uint64>(data_); }

	void setValue(uint64 v)
	{
		uint64 old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<uint64>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<uint64> eventHandler_;
};

template<> struct Property<float> : public IProperty
{
	Property(std::string_view name, float v) : IProperty(name, VR_DATA_TYPE::FLOAT) { data_ = v; }
	const float& getValue() { return std::get<float>(data_); }

	void setValue(float v)
	{
		float old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<float>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<float> eventHandler_;
};

template<> struct Property<double> : public IProperty
{
	Property(std::string_view name, double v) : IProperty(name, VR_DATA_TYPE::DOUBLE) { data_ = v; }
	const double& getValue() { return std::get<double>(data_); }

	void setValue(double v)
	{
		double old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<double>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<double> eventHandler_;
};

template<> struct Property<std::string> : public IProperty
{
	Property(std::string_view name, const std::string& v) : IProperty(name, VR_DATA_TYPE::STRING) { data_ = v; }
	const std::string& getValue() { return std::get<std::string>(data_); }

	void setValue(const std::string& v)
	{
		std::string old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<std::string>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<std::string> eventHandler_;
};

template<> struct Property<uuid> : public IProperty
{
	Property(std::string_view name, const uuid& v) : IProperty(name, VR_DATA_TYPE::UUID) { data_ = v; }
	const uuid& getValue() { return std::get<uuid>(data_); }

	void setValue(const uuid& v)
	{
		uuid old_v = getValue();
		if (old_v != v)
		{
			data_ = v;
			eventHandler_.onEventHandler(name_, old_v, v);
		}
	}

	void registerCallback(EventHandler<uuid>::EventFun&& cb)
	{
		eventHandler_.registerCallback(std::move(cb));
	}

protected:
	EventHandler<uuid> eventHandler_;
};


class PropertyArray
{
	using EventFun = std::function<void(std::string_view name, IPropertyPtr old_prop, IPropertyPtr new_prop)>;
public:
	PropertyArray(std::string_view name)
		:name_(name)
	{
	}

	virtual ~PropertyArray() = default;

	//void setProperty(IPropertyPtr prop, size_t pos)
	//{
	//	if (prop == nullptr || pos < 0 || pos >= vecVData_.size())
	//	{
	//		return;
	//	}

	//	//if ()
	//}

	void appenProperty(IPropertyPtr prop)
	{
		if (prop != nullptr)
		{
			vecVData_.emplace_back(prop);
		}
	}

	IPropertyPtr operator[](size_t pos) 
	{ 
		if (getSize() <= pos)
		{
			return nullptr;
		}
		
		return vecVData_[pos]; 
	}

	const std::string& getName() const { return name_; }
	size_t getSize() const { return vecVData_.size(); }
	//const std::vector<VariantData>& getData() const { return vecVData_; }

protected:


//	void setData(VariantData&& vdata, size_t pos)
//	{
//		// 类型检查
//		if (invalidDataType(vdata.type_))
//		{
//			return;
//		}
//
//		// pos检查, 这里pos是从0开始
//		if (pos >= vecVData_.size())
//		{
//			return;
//		}
//
//		// 这里不检查前后的类型
//		if (vecVData_[0] != vdata)
//		{
//			VariantData old_data = vecVData_[0];
//			vecVData_[0] = std::move(vdata);
//			onEventHandler(old_data, vecVData_[0], pos);
//		}
//	}
//
//	void onEventHandler(const VariantData& oldVar, const VariantData& newVar, size_t pos)
//	{
//		for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
//		{
//			(*it)(propertyName_, oldVar, newVar, pos);
//		}
//	}
//
private:

	std::string name_;
	//VecArrayPropertyCbT propertyCb_;
	std::vector<IPropertyPtr> vecVData_;
};

}
