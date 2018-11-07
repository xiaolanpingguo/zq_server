#pragma once

#include "data_list.hpp"
#include "IRecord.h"
#include "IRecordManager.h"
#include "IPropertyManager.h"
#include "baselib/interface_header/platform.h"
#include "baselib/interface_header/ILibManager.h"

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

class ZQ_EXPORT IObject :public MemoryCounter<IObject>
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
	virtual bool addRecordCallBack(const std::string& strRecordName, RECORD_EVENT_FUNCTOR&& cb) = 0;
	virtual bool addPropertyCallBack(const std::string& strPropertyName, PROPERTY_EVENT_FUNCTOR&& cb) = 0;

	virtual CLASS_OBJECT_EVENT getState() = 0;
	virtual bool setState(const CLASS_OBJECT_EVENT eState) = 0;

	virtual bool findProperty(const std::string& strPropertyName) = 0;

	virtual bool setPropertyInt(const std::string& strPropertyName, const int64 nValue) = 0;
	virtual bool setPropertyFloat(const std::string& strPropertyName, const double dwValue) = 0;
	virtual bool setPropertyString(const std::string& strPropertyName, const std::string& strValue) = 0;
	virtual bool setPropertyObject(const std::string& strPropertyName, const Guid& obj) = 0;

	virtual int64 getPropertyInt(const std::string& strPropertyName) = 0;
	virtual double getPropertyFloat(const std::string& strPropertyName) = 0;
	virtual const std::string& getPropertyString(const std::string& strPropertyName) = 0;
	virtual const Guid& getPropertyObject(const std::string& strPropertyName) = 0;

	virtual bool findRecord(const std::string& strRecordName) = 0;

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue) = 0;
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj) = 0;

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value) = 0;
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value) = 0;

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

	virtual std::shared_ptr<IRecordManager> getRecordManager() = 0;
	virtual std::shared_ptr<IPropertyManager> getPropertyManager() = 0;
	virtual void setRecordManager(std::shared_ptr<IRecordManager> xRecordManager) = 0;
	virtual void setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager) = 0;

protected:
	ILibManager * m_pPluginManager;
};

}
