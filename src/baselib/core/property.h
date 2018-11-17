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
	using VecPropertyCbT = std::vector<PropertyEventFunT>;
public:
	Property(const std::string& strPropertyName, const EN_DATA_TYPE varType)
		:propertyName_(strPropertyName),
		dataType_(varType) 
	{
	}

	virtual ~Property() = default;

	template <typename T>
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
	}

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type getValue()
	{
		if (isIntegralType(getType()))
		{
			return (T)vData_.variantData_.get<int64>();
		}
		else
		{
			return VALID_INT;
		}
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type getValue()
	{
		if (isFloatingType(getType()))
		{
			return (T)vData_.variantData_.get<double>();
		}
		else
		{
			return VALID_FLOAT;
		}
	}

	template <typename T>
	typename std::enable_if<std::is_same<std::string, T>::value, T>::type getValue()
	{
		if (isStringType(getType()))
		{
			return (T)vData_.variantData_.get<std::string>();
		}
		else
		{
			return NULL_STR;
		}
	}

	template <typename T>
	typename std::enable_if<std::is_same<Guid, T>::value, T>::type getValue()
	{
		if (isStringType(getType()))
		{
			return (T)vData_.variantData_.get<Guid>();
		}
		else
		{
			return NULL_GUID;
		}
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
		setData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, const char*>::value>::type setValue(const T& v, size_t pos)
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
	typename std::enable_if<std::is_same<T, std::string>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_STRING);
		vd.variantData_ = (std::string)(v);
		setData(std::move(vd));
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, Guid>::value>::type setValue(const T& v, size_t pos)
	{
		VariantData	vd(TDATA_OBJECT);
		vd.variantData_ = (Guid)v;
		setData(std::move(vd));
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
