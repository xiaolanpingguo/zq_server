#pragma once


//#include "map_ex.hpp"
//#include "data_list.hpp"
//#include "IPropertyManager.h"
#include "property.h"

#include <unordered_map>

namespace zq {

class PropertyManager 
{
public:

	PropertyManager() 
	{
	}

	virtual ~PropertyManager()
	{
		propertys_.clear();
		propertyArray_.clear();
	};

	PropertyPtr newProperty(const std::string& strPropertyName, const EN_DATA_TYPE varType)
	{
		auto it = propertys_.find(strPropertyName);
		if (it == propertys_.end())
		{
			auto new_prop = std::make_shared<Property>(strPropertyName, varType);
			propertys_[strPropertyName] = new_prop;
			return new_prop;
		}

		return it->second;
	}

	PropertyPtr getProperty(const std::string& strPropertyName)
	{
		auto it = propertys_.find(strPropertyName);
		if (it == propertys_.end())
		{
			return nullptr;
		}

		return it->second;
	}

	void appenArrayPropertyData(const std::string& strPropertyName, int64 value)
	{
		PropertyArrayPtr prop_arry = nullptr;
		auto it = propertyArray_.find(strPropertyName);
		if (it == propertyArray_.end())
		{
			prop_arry = std::make_shared<PropertyArray>(strPropertyName);
		}

		prop_arry->appenValue(value);
		propertyArray_[strPropertyName] = prop_arry;
	}

	void setArrayPropertyDataOnPos(const std::string& strPropertyName, int64 value, size_t pos)
	{
		auto it = propertyArray_.find(strPropertyName);
		if (it == propertyArray_.end())
		{
			return;
		}

		PropertyArrayPtr prop_arry = it->second;
		prop_arry->setValue(value, pos);
	}

	void getArrayProperty(const std::string& strPropertyName, std::vector<PropertyPtr>& out, int begin = -1, int end = -1)
	{
		//auto it = propertyArray_.find(strPropertyName);
		//if (it == propertyArray_.end())
		//{
		//	return;
		//}

		//// begin和end都以下标来计算
		//std::vector<PropertyPtr>& vec = it->second;
		//if (begin == -1 && end == -1)
		//{
		//	for (const auto& prop : vec)
		//	{
		//		out.emplace_back(prop);
		//	}

		//	return;
		//}

		//if (begin == -1 && end != -1)
		//{
		//	if ((int)vec.size() > end)
		//	{
		//		for (size_t i = 0; i <= end; ++i)
		//		{
		//			out.emplace_back(vec[i]);
		//		}
		//	}

		//	return;
		//}

		//if (begin != -1 && end == -1)
		//{
		//	if ((int)vec.size() > begin)
		//	{
		//		for (size_t i = begin; i < vec.size(); ++i)
		//		{
		//			out.emplace_back(vec[i]);
		//		}
		//	}

		//	return;
		//}

		//if (begin != -1 && end != -1)
		//{	
		//	if ((int)vec.size() > end && (int)vec.size() > begin)
		//	{
		//		for (size_t i = begin; i < end; ++i)
		//		{
		//			out.emplace_back(vec[i]);
		//		}
		//	}

		//	return;
		//}
	}

	bool exsitProperty(const std::string& name)
	{
		return exsitBaseProperty(name) || exsitArrayProperty(name);
	}

	bool exsitBaseProperty(const std::string& name)
	{
		return propertyArray_.find(name) != propertyArray_.end();
	}

	bool exsitArrayProperty(const std::string& name)
	{
		return propertyArray_.find(name) != propertyArray_.end();
	}

	bool registerCallback(const std::string& strPropertyName, PropertyEventFunT&& cb)
	{
		auto it = propertys_.find(strPropertyName);
		if (it == propertys_.end())
		{
			return false;
		}

		it->second->registerCallback(std::move(cb));
		return true;
	}

private:

	std::unordered_map<std::string, PropertyPtr> propertys_;   // 基本类型
	std::unordered_map<std::string, PropertyArrayPtr> propertyArray_;  // 数组类型
};

}
