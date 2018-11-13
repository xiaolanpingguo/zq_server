#pragma once


#include "map_ex.hpp"
#include "data_list.hpp"
#include "IPropertyManager.h"

namespace zq {

class PropertyManager : public IPropertyManager
{
public:

	PropertyManager()
	{

	}
	PropertyManager(const Guid& self)
	{
		mSelf = self;
	};

	virtual ~PropertyManager();

	virtual bool registerCallback(const std::string& strProperty, PROPERTY_EVENT_FUNCTOR&& cb);

	virtual std::shared_ptr<IProperty> addProperty(const Guid& self, std::shared_ptr<IProperty> pProperty);

	virtual std::shared_ptr<IProperty> addProperty(const Guid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType);

	virtual const Guid& self();

	virtual bool setProperty(const std::string& strPropertyName, const VariantData& TData);

	virtual bool setPropertyInt(const std::string& strPropertyName, const int64 nValue);
	virtual bool setPropertyDouble(const std::string& strPropertyName, const double dwValue);
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue);
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj);

	virtual int64 getPropertyInt(const std::string& strPropertyName);
	virtual double getPropertyDouble(const std::string& strPropertyName);
	virtual const std::string& getPropertyString(const std::string& strPropertyName);
	virtual const Guid& getPropertyObject(const std::string& strPropertyName);

private:
	Guid mSelf;
	std::map<std::string, int> mxPropertyIndexMap;
};

}
