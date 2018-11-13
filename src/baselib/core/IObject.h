#pragma once


#include "data_list.hpp"
#include "IPropertyManager.h"
#include "interface_header/base/platform.h"
#include "interface_header/base/ILibManager.h"

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

class IObject :public MemoryCounter<IObject>
{
private:
	IObject() : MemoryCounter(GET_CLASS_NAME(IObject))
	{
	}

public:
	IObject(Guid self) : MemoryCounter(GET_CLASS_NAME(IObject))
	{
	}

	virtual ~IObject()
	{
	}

	virtual bool run() = 0;

	virtual Guid self() = 0;

	// »Øµ÷
	virtual bool addPropertyCallBack(const std::string& strPropertyName, PROPERTY_EVENT_FUNCTOR&& cb) = 0;

	virtual CLASS_OBJECT_EVENT getState() = 0;
	virtual bool setState(const CLASS_OBJECT_EVENT eState) = 0;

	virtual bool findProperty(const std::string& strPropertyName) = 0;

	virtual bool setPropertyInt(const std::string& strPropertyName, const int64 nValue) = 0;
	virtual bool setPropertyDouble(const std::string& strPropertyName, const double dwValue) = 0;
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue) = 0;
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj) = 0;

	virtual int64 getPropertyInt(const std::string& strPropertyName) = 0;
	virtual double getPropertyDouble(const std::string& strPropertyName) = 0;
	virtual const std::string& getPropertyString(const std::string& strPropertyName) = 0;
	virtual const Guid& getPropertyObject(const std::string& strPropertyName) = 0;

	virtual std::shared_ptr<IPropertyManager> getPropertyMgr() = 0;
	virtual void setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager) = 0;

protected:
	ILibManager * m_pPluginManager;
};

}
