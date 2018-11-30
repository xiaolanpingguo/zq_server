#pragma once


#include <list>
#include "data_list.hpp"
#include "interface_header/base/platform.h"

namespace zq {

class Property;
class PropertyArray;

using PropertyPtr = std::shared_ptr<Property>;
using PropertyArrayPtr = std::shared_ptr<PropertyArray>;

using PropertyEventFunT = std::function<void(const std::string&, const VariantData&, const VariantData&)>;
using ArrayPropertyEventFunT = std::function<void(const std::string&, const VariantData&, const VariantData&, size_t pos)>;

class Property
{
	//using VariantData = std::variant<int64, double, std::string, Guid>;
	using VecPropertyCbT = std::vector<PropertyEventFunT>;
public:
	Property(const std::string& strPropertyName, const EN_DATA_TYPE varType)
		:propertyName_(strPropertyName),
		dataType_(varType) 
	{
	}

	Property(EN_DATA_TYPE varType = TDATA_UNKNOWN): dataType_(varType) {}
	Property(const Property& prop) : dataType_(prop.dataType_), vData_(prop.vData_) {}
	Property(Property&& prop) : dataType_(prop.dataType_), vData_(std::move(prop.vData_)){}

	Property& operator=(const Property& right)
	{
		if (this != &right)
		{
			dataType_ = right.dataType_;
			vData_ = right.vData_;
		}

		return *this;
	}

	Property& operator=(Property&& right)
	{
		if (this != &right)
		{
			dataType_ = right.dataType_;
			vData_ = std::move(right.vData_);
		}

		return *this;
	}

	bool operator==(const Property& right)
	{
		if (right.dataType_ == this->dataType_)
		{
			return vData_ == vData_;
		}

		return false;
	}

	virtual ~Property() = default;

	template <typename T>
	void setValue(const T& v)
	{
		if constexpr (std::is_integral_v<T>)
		{
			VariantData	vd(TDATA_INT64);
			vd.variantData_ = (int64)v;
			setData(std::move(vd));
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			VariantData	vd(TDATA_DOUBLE);
			vd.variantData_ = (double)v;
			setData(std::move(vd));
		}
		else if constexpr (std::is_same_v<std::string, T>)
		{
			VariantData	vd(TDATA_STRING);
			vd.variantData_ = (std::string)(v);
			setData(std::move(vd));
		}
		else if constexpr (std::is_same_v<Guid, T>)
		{
			VariantData	vd(TDATA_GUID);
			vd.variantData_ = (Guid)v;
			setData(std::move(vd));
		}
		else
		{
			static_assert(false, "property: unsupport type!");
		}
	}

	//template <typename T>
	//decltype(auto) getValue()
	//{
	//	if constexpr (std::is_integral_v<T>)
	//	{
	//		if (isIntegralType(getType()))
	//			return vData_.variantData_.get<int64>();
	//		
	//		printError(getType());
	//		return VALID_INT;
	//	}
	//	else if constexpr (std::is_floating_point_v<T>)
	//	{
	//		if (isFloatingType(getType()))
	//			return vData_.variantData_.get<double>();

	//		printError(getType());
	//		return VALID_FLOAT;
	//	}
	//	else if constexpr (std::is_same_v<std::string, T>)
	//	{
	//		if (isStringType(getType()))
	//			return vData_.variantData_.get<std::string>();

	//		printError(getType());
	//		return NULL_STR;
	//	}
	//	else if constexpr (std::is_same_v<Guid, T>)
	//	{
	//		if (isGuidType(getType()))
	//			return vData_.variantData_.get<Guid>();

	//		printError(getType());
	//		return NULL_GUID;
	//	}
	//	else
	//	{
	//		static_assert(false, "property: unsupport type!");
	//	}
	//}

	void printError(EN_DATA_TYPE type)
	{
		LOG_ERROR << fmt::format("type error, type: {}", dataType2Str(type));
	}








	/*template <typename T>
	typename std::enable_if<std::is_integral<T>::value>::type setValue(const T& v)
	{
		VariantData	vd(TDATA_INT64);
		vd.variantData_ = (int64)v;
		setData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value>::type setValue(const T& v)
	{
		VariantData	vd(TDATA_DOUBLE);
		vd.variantData_ = (double)v;
		setData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, const char*>::value>::type setValue(const T& v)
	{
		if (v)
		{
			VariantData	vd(TDATA_STRING);
			vd.variantData_ = std::string(v);
			setData(std::move(vd));
		}

		return nullptr;
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, std::string>::value>::type setValue(const T& v)
	{
		VariantData	vd(TDATA_STRING);
		vd.variantData_ = (std::string)(v);
		setData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, Guid>::value>::type setValue(const T& v)
	{
		VariantData	vd(TDATA_OBJECT);
		vd.variantData_ = (Guid)v;
		setData(std::move(vd));
	}*/

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type getValue()
	{
		if (isIntegralType(getType()))
			return std::get<int64>(vData_.variantData_);

		printError(getType());
		return VALID_INT;
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type getValue()
	{
		if (isFloatingType(getType()))
			return std::get<double>(vData_.variantData_);

		printError(getType());
		return VALID_FLOAT;
	}

	template <typename T>
	typename std::enable_if<std::is_same<std::string, T>::value, T>::type getValue()
	{
		if (isStringType(getType()))
			return std::get<std::string>(vData_.variantData_);

		printError(getType());
		return NULL_STR;
	}

	template <typename T>
	typename std::enable_if<std::is_same<Guid, T>::value, T>::type getValue()
	{
		if (isGuidType(getType()))
			return std::get<Guid>(vData_.variantData_);

		printError(getType());
		return NULL_GUID;
	}

	virtual const EN_DATA_TYPE getType() const { return dataType_; }
	virtual const std::string& getName() const { return propertyName_; }

	void registerCallback(PropertyEventFunT&& cb)
	{
		propertyCb_.emplace_back(std::move(cb));
	}

protected:

	void setData(VariantData&& vdata)
	{
		if (invalidDataType(vdata.type_))
		{
			return;
		}

		// 前后设置的类型要一致
		if (dataType_ != TDATA_UNKNOWN && dataType_ != vdata.type_)
		{
			return;
		}

		dataType_ = vdata.type_;

		if (propertyCb_.empty())
		{
			vData_ = std::move(vdata);
		}
		else
		{
			// 值变化了，回调事件
			VariantData oldValue;
			oldValue = vData_;
			vData_ = std::move(vdata);

			onEventHandler(oldValue, vData_);
		}
	}

	void onEventHandler(const VariantData& oldVar, const VariantData& newVar)
	{
		for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
		{
			(*it)(propertyName_, oldVar, newVar);
		}
	}

private:

	std::string propertyName_;
	EN_DATA_TYPE dataType_;
	VecPropertyCbT propertyCb_;

	VariantData vData_;
};


class PropertyArray
{
	using VecPropertyCbT = std::vector<PropertyEventFunT>;
	using VecArrayPropertyCbT = std::vector<ArrayPropertyEventFunT>;
public:
	PropertyArray(const std::string& strPropertyName)
		:propertyName_(strPropertyName)
	{
	}

	virtual ~PropertyArray() = default;

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value>::type appenValue(const T& v)
	{
		VariantData	vd(TDATA_INT64);
		vd.variantData_ = (int64)v;
		appendData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value>::type appenValue(const T& v)
	{
		VariantData	vd(TDATA_DOUBLE);
		vd.variantData_ = (double)v;
		appendData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, const char*>::value>::type appenValue(const T& v)
	{
		if (v)
		{
			VariantData	vd(TDATA_STRING);
			vd.variantData_ = std::string(v);
			appendData(std::move(vd));
		}

		return nullptr;
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, std::string>::value>::type appenValue(const T& v)
	{
		VariantData	vd(TDATA_STRING);
		vd.variantData_ = (std::string)(v);
		appendData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, Guid>::value>::type appenValue(const T& v)
	{
		VariantData	vd(TDATA_OBJECT);
		vd.variantData_ = (Guid)v;
		appendData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_INT64);
		vd.variantData_ = (int64)v;
		setData(std::move(vd), pos);
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_DOUBLE);
		vd.variantData_ = (double)v;
		setData(std::move(vd), pos);
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, const char*>::value>::type setValue(const T& v, size_t pos)
	{
		if (v)
		{
			VariantData	vd(TDATA_STRING);
			vd.variantData_ = std::string(v);
			setData(std::move(vd), pos);
		}

		return nullptr;
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, std::string>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_STRING);
		vd.variantData_ = (std::string)(v);
		setData(std::move(vd), pos);
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, Guid>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_OBJECT);
		vd.variantData_ = (Guid)v;
		setData(std::move(vd), pos);
	}

	const std::string& getName() const { return propertyName_; }
	const std::vector<VariantData>& getVecData() const { return vecVData_; }

protected:

	void appendData(VariantData&& vdata)
	{
		// 类型检查
		if (invalidDataType(vdata.type_))
		{
			return;
		}

		vecVData_.emplace_back(std::move(vdata));
	}

	void setData(VariantData&& vdata, size_t pos)
	{
		// 类型检查
		if (invalidDataType(vdata.type_))
		{
			return;
		}

		// pos检查, 这里pos是从0开始
		if (pos >= vecVData_.size())
		{
			return;
		}

		// 这里不检查前后的类型
		if (vecVData_[0] != vdata)
		{
			VariantData old_data = vecVData_[0];
			vecVData_[0] = std::move(vdata);
			onEventHandler(old_data, vecVData_[0], pos);
		}
	}

	void onEventHandler(const VariantData& oldVar, const VariantData& newVar, size_t pos)
	{
		for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
		{
			(*it)(propertyName_, oldVar, newVar, pos);
		}
	}

private:

	std::string propertyName_;

	VecArrayPropertyCbT propertyCb_;
	std::vector<VariantData> vecVData_;
};

}
