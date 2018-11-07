#pragma once

#include "map_ex.hpp"
#include "IProperty.h"
#include "baselib/interface_header/platform.h"

namespace zq {

class ZQ_EXPORT IPropertyManager : public MapEx<std::string, IProperty>
{
public:
	virtual ~IPropertyManager() {}

	virtual bool registerCallback(const std::string& strProperty, PROPERTY_EVENT_FUNCTOR&& cb) = 0;

	virtual std::shared_ptr<IProperty> addProperty(const Guid& self, std::shared_ptr<IProperty> pProperty) = 0;

	virtual std::shared_ptr<IProperty> addProperty(const Guid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType) = 0;

	virtual const Guid& self() = 0;

	virtual bool setProperty(const std::string& strPropertyName, const AbstractData& TData) = 0;

	virtual bool setPropertyInt(const std::string& strPropertyName, const int64 nValue) = 0;
	virtual bool setPropertyFloat(const std::string& strPropertyName, const double dwValue) = 0;
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue) = 0;
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj) = 0;

	virtual int64 getPropertyInt(const std::string& strPropertyName) = 0;
	virtual double getPropertyFloat(const std::string& strPropertyName) = 0;
	virtual const std::string& getPropertyString(const std::string& strPropertyName) = 0;
	virtual const Guid& getPropertyObject(const std::string& strPropertyName) = 0;
};

}
