#pragma once


#include "property.h"
#include "memory_counter.hpp"
#include "interface_header/base/platform.h"
#include "interface_header/base/uuid.h"

namespace zq {

enum CLASS_OBJECT_EVENT
{
	COE_CREATE_NODATA,
	COE_CREATE_BEFORE_ATTACHDATA,
	COE_CREATE_LOADDATA,
	COE_CREATE_AFTER_ATTACHDATA,
	COE_CREATE_BEFORE_EFFECT,
	COE_CREATE_EFFECTDATA,
	COE_CREATE_AFTER_EFFECT,
	COE_CREATE_HASDATA,
	COE_CREATE_FINISH,
	COE_CREATE_CLIENT_FINISH,
	COE_BEFOREDESTROY,
	COE_DESTROY,
};

class IObject;
using IObjectPtr = std::shared_ptr<IObject>;
class IObject :public MemoryCounter<IObject>
{
public:
	IObject() : MemoryCounter(GET_CLASS_NAME(IObject))
	{
	}

	virtual ~IObject()
	{
	}

	virtual const Guid& getGuid() = 0;
	virtual const std::string& getObjName() = 0;
	virtual const std::string& getClassName() = 0;

	virtual bool exsitProperty(const std::string& strPropertyName) = 0;

	virtual bool setValue(const std::string& strPropertyName, const int64 nValue) = 0;
	virtual bool setValue(const std::string& strPropertyName, const double dwValue) = 0;
	virtual bool setValue(const std::string& strPropertyName, const std::string& strValue) = 0;
	virtual bool setValue(const std::string& strPropertyName, const Guid& obj) = 0;

	virtual int64 getInt(const std::string& strPropertyName) = 0;
	virtual double getDouble(const std::string& strPropertyName) = 0;
	virtual std::string getString(const std::string& strPropertyName) = 0;

	virtual bool addPropertyCallBack(const std::string& name, PropertyEventFunT&& cb) = 0;
};

}
