#pragma once


#include "property_manager.h"
#include "IObject.h"

namespace zq {

class CObject : public IObject
{
public:
	CObject(Guid id = NULL_GUID, const std::string& name = "")
		:self_(id),name_(name)
	{

	}
	virtual ~CObject()
	{

	}

	virtual bool exsitProperty(const std::string& strPropertyName)
	{
		return propertyMgr_.exsitProperty(strPropertyName);
	}

	bool setValue(const std::string& strPropertyName, const int64 value) override
	{
		PropertyPtr pProperty = propertyMgr_.newProperty(strPropertyName, TDATA_INT64);
		pProperty->setValue(value);
		return true;
	}

	bool setValue(const std::string& strPropertyName, const double value) override
	{
		PropertyPtr pProperty = propertyMgr_.newProperty(strPropertyName, TDATA_DOUBLE);
		pProperty->setValue(value);
		return true;
	}

	bool setValue(const std::string& strPropertyName, const std::string& value) override
	{
		PropertyPtr pProperty = propertyMgr_.newProperty(strPropertyName, TDATA_STRING);
		pProperty->setValue(value);
		return true;
	}

	bool setValue(const std::string& strPropertyName, const Guid& value) override
	{
		PropertyPtr pProperty = propertyMgr_.newProperty(strPropertyName, TDATA_GUID);
		pProperty->setValue(value);
		return true;
	}

	int64 getInt(const std::string& strPropertyName) override
	{
		PropertyPtr pProperty = propertyMgr_.getProperty(strPropertyName);
		if (pProperty == nullptr)
		{
			return VALID_INT;
		}

		return pProperty->getValue<int64>();
	}

	double getDouble(const std::string& strPropertyName) override
	{
		PropertyPtr pProperty = propertyMgr_.getProperty(strPropertyName);
		if (pProperty == nullptr)
		{
			return VALID_FLOAT;
		}

		return pProperty->getValue<double>();
	}

	std::string getString(const std::string& strPropertyName) override
	{
		PropertyPtr pProperty = propertyMgr_.getProperty(strPropertyName);
		if (pProperty == nullptr)
		{
			return NULL_STR;
		}

		return pProperty->getValue<std::string>();
	}

	bool setArrayValueInt(const std::string& strPropertyName, const std::vector<int64>& vec)
	{
		/*for (size_t i = 0; i < vec.size(); ++i)
		{
			propertyMgr_.setArrayProperty(strPropertyName, vec);
			if (pProperty == nullptr)
			{
				pProperty = std::make_shared<Property>(strPropertyName, TDATA_STRING);
			}
		}*/

		return true;
	}

	void getArrayValueInt(const std::string& strPropertyName, std::vector<int64>& vec)
	{
		std::vector<PropertyPtr> out;
		propertyMgr_.getArrayProperty(strPropertyName, out);

		for (size_t i = 0; i < out.size(); ++i)
		{
			vec.emplace_back(out[i]->getValue<int64>());
		}
	}

	void getArrayValueDouble(const std::string& strPropertyName, std::vector<double>& vec)
	{
		std::vector<PropertyPtr> out;
		propertyMgr_.getArrayProperty(strPropertyName, out);

		for (size_t i = 0; i < out.size(); ++i)
		{
			vec.emplace_back(out[i]->getValue<double>());
		}
	}

	void getArrayValueDouble(const std::string& strPropertyName, std::vector<std::string>& vec)
	{
		std::vector<PropertyPtr> out;
		propertyMgr_.getArrayProperty(strPropertyName, out);

		for (size_t i = 0; i < out.size(); ++i)
		{
			vec.emplace_back(out[i]->getValue<std::string>());
		}
	}

	void getArrayValueGuid(const std::string& strPropertyName, std::vector<Guid>& vec)
	{
		std::vector<PropertyPtr> out;
		propertyMgr_.getArrayProperty(strPropertyName, out);

		for (size_t i = 0; i < out.size(); ++i)
		{
			vec.emplace_back(out[i]->getValue<Guid>());
		}
	}

	void getArrayValueInt(const std::string& strPropertyName, std::vector<int64>& vec, size_t size)
	{
		std::vector<PropertyPtr> out;
		propertyMgr_.getArrayProperty(strPropertyName, out);

		for (size_t i = 0; i < out.size(); ++i)
		{
			vec.emplace_back(out[i]->getValue<int64>());
		}
	}

	bool addPropertyCallBack(const std::string& name, PropertyEventFunT&& cb) override
	{
		return propertyMgr_.registerCallback(name, std::move(cb));
	}

	const Guid& getGuid() override { return self_; }

	void setObjName(const std::string& v) { name_ = v; }
	const std::string& getObjName() override { return name_; }

	void setClassName(const std::string& v) { name_ = v; }
	const std::string& getClassName() override { return name_; }

private:

	Guid self_;
	std::string name_;
	PropertyManager propertyMgr_;
};

}
