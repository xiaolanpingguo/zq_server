#pragma once

#include <iostream>
#include <string>
#include "baselib/core/IObject.h"
#include "baselib/core/IRecordManager.h"
#include "baselib/core/IPropertyManager.h"
#include "baselib/interface_header/platform.h"

namespace zq {

class ZQ_EXPORT CObject : public IObject
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
	virtual bool setPropertyFloat(const std::string& strPropertyName, const double dwValue);
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue);
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj);

	virtual int64 getPropertyInt(const std::string& strPropertyName);
	virtual double getPropertyFloat(const std::string& strPropertyName);
	virtual const std::string& getPropertyString(const std::string& strPropertyName);
	virtual const Guid& getPropertyObject(const std::string& strPropertyName);

	virtual bool findRecord(const std::string& strRecordName);

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue);
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue);
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue);
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj);

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value);
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value);


	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol);
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const int nCol);
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const int nCol);

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);

	virtual std::shared_ptr<IRecordManager> getRecordManager();
	virtual std::shared_ptr<IPropertyManager> getPropertyManager();

	virtual void setRecordManager(std::shared_ptr<IRecordManager> xRecordManager);
	virtual void setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager);

protected:
	virtual bool addRecordCallBack(const std::string& strRecordName, RECORD_EVENT_FUNCTOR&& cb);

	virtual bool addPropertyCallBack(const std::string& strCriticalName, PROPERTY_EVENT_FUNCTOR&& cb);

private:
	Guid mSelf;
	CLASS_OBJECT_EVENT mObjectEventState;
	std::shared_ptr<IRecordManager> recordManager_;
	std::shared_ptr<IPropertyManager> propertyManager_;

};

}
