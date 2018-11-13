#pragma once

#include <iostream>
#include <string>
#include "baselib/core/IObject.h"
#include "baselib/core/IPropertyManager.h"
#include "interface_header/base/platform.h"

namespace zq {

class CObject : public IObject
{
private:
	CObject() : IObject(Guid())
	{
		mObjectEventState = COE_CREATE_NODATA;
	}

public:
	CObject(Guid self, ILibManager* pLuginManager);
	virtual ~CObject();

	virtual bool init();
	virtual bool shut();
	virtual bool run();

	virtual Guid self();

	virtual CLASS_OBJECT_EVENT getState();
	virtual bool setState(const CLASS_OBJECT_EVENT eState);

	virtual bool findProperty(const std::string& strPropertyName);

	virtual bool setPropertyInt(const std::string& strPropertyName, const int64 nValue);
	virtual bool setPropertyDouble(const std::string& strPropertyName, const double dwValue);
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue);
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj);

	virtual int64 getPropertyInt(const std::string& strPropertyName);
	virtual double getPropertyDouble(const std::string& strPropertyName);
	virtual const std::string& getPropertyString(const std::string& strPropertyName);
	virtual const Guid& getPropertyObject(const std::string& strPropertyName);

	virtual std::shared_ptr<IPropertyManager> getPropertyMgr();

	virtual void setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager);

protected:

	virtual bool addPropertyCallBack(const std::string& strCriticalName, PROPERTY_EVENT_FUNCTOR&& cb);

private:
	Guid mSelf;
	CLASS_OBJECT_EVENT mObjectEventState;
	std::shared_ptr<IPropertyManager> propertyManager_;

};

}
