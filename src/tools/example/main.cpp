#include <iostream>
#include <random>
#include <variant>
#include <unordered_map>
#include <map>
#include "test_format.hpp"
//#include "baselib/core/property.h"
#include "interface_header/base/platform.h"


enum class VR_DATA_TYPE
{
	INT8, INT16, UINT16, INT32, UINT32, INT64, UINT64,
	FLOAT, DOUBLE, STRING, UUID,
};

struct IProperty
{
	template <typename T>
	using PropertyEventFun = std::function<void(const std::string&, const T&, const T&)>;

protected:
	IProperty(std::string_view name, VR_DATA_TYPE type) : name_(name), type_(type) {}
	virtual ~IProperty() = default;

	VR_DATA_TYPE type_;
	std::string name_;
	std::variant<int8, int16, uint16, int32, uint32, int64, uint64,
		float, double, std::string> data_;
};


template<typename T> struct Property;

//template<> struct Property<int8> : public IProperty
//{
//	Property(std::string_view name, int8 v) : IProperty(name, VR_DATA_TYPE::INT8) { setValue(v); }
//	void setValue(int8 v) { data_ = v; }
//	int8 getValue() { return std::get<int8>(data_); }
//
//	void registerCallback(PropertyEventFun<int8>&& cb)
//	{
//		propertyCb_.emplace_back(std::move(cb));
//	}
//
//protected:
//	std::vector<PropertyEventFun<int8>> propertyCb_;
//};
//
//template<> struct Property<int16> : public IProperty
//{
//	Property(std::string_view name, int16 v) : IProperty(name, VR_DATA_TYPE::INT16) { setValue(v); }
//	void setValue(int16 v) { data_ = v; }
//	int16 getValue() { return std::get<int16>(data_); }
//};
//
//template<> struct Property<uint16> : public IProperty
//{
//	Property(std::string_view name, uint16 v) : IProperty(name, VR_DATA_TYPE::UINT16) { setValue(v); }
//	void setValue(uint16 v) { data_ = v; }
//	uint16 getValue() { return std::get<uint16>(data_); }
//};
//
//template<> struct Property<int32> : public IProperty
//{
//	Property(std::string_view name, int32 v) : IProperty(name, VR_DATA_TYPE::INT32) { setValue(v); }
//	void setValue(int32 v) { data_ = v; }
//	int32 getValue() { return std::get<int32>(data_); }
//};
//
//template<> struct Property<uint32> : public IProperty
//{
//	Property(std::string_view name, uint32 v) : IProperty(name, VR_DATA_TYPE::UINT32) { setValue(v); }
//	void setValue(uint32 v) { data_ = v; }
//	uint32 getValue() { return std::get<uint32>(data_); }
//};
//
template<> struct Property<int64> : public IProperty
{
	Property(std::string_view name, int64 v) : IProperty(name, VR_DATA_TYPE::INT64) { setValue(v); }
	void setValue(int64 v) { data_ = v; }
	int64 getValue() { return std::get<int64>(data_); }
};

template<> struct Property<uint64> : public IProperty
{
	Property(std::string_view name, uint64 v) : IProperty(name, VR_DATA_TYPE::UINT64) { setValue(v); }
	void setValue(uint64 v) { data_ = v; }
	uint64 getValue() { return std::get<uint64>(data_); }
};

//template<> struct Property<float> : public IProperty
//{
//	Property(std::string_view name, float v) : IProperty(name, VR_DATA_TYPE::FLOAT) { setValue(v); }
//	void setValue(float v) { data_ = v; }
//	float getValue() { return std::get<float>(data_); }
//};
//
//template<> struct Property<double> : public IProperty
//{
//	Property(std::string_view name, double v) : IProperty(name, VR_DATA_TYPE::DOUBLE) { setValue(v); }
//	void setValue(double v) { data_ = v; }
//	double getValue() { return std::get<double>(data_); }
//};
//
//template<> struct Property<std::string> : public IProperty
//{
//	Property(std::string_view name, const std::string& v) : IProperty(name, VR_DATA_TYPE::STRING) { setValue(v); }
//	void setValue(const std::string& v) { data_ = v; }
//	const std::string& getValue() { return std::get<std::string>(data_); }
//};



std::map<std::string, IProperty*> aa;



template <typename T>
auto getProperty()
{
	if constexpr (std::is_same_v<T, int64>)
	{
		return static_cast<Property<int64>*>(aa[""]);
	}
	else if constexpr (std::is_same_v<T, uint64>)
	{
		return static_cast<Property<uint64>*>(aa[""]);
	}
}


int main()
{
	bool a = true;
	auto prop = new Property<int64>("123", a);
	getProperty<int64>();
	//prop->registerCallback([](const std::string&, int8 oldv, int8 newv)
	//{

	//});
	
	//Property prop(TDATA_INT64);
	//prop.setValue(100);
	//cout << prop.getValue<int64>() << endl;
	//test_format();
	std::getchar();
	return 0;
}





