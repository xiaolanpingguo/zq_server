#pragma once

#include <iostream>
#include <string>
#include <functional>
#include "IModule.h"
#include "lib/core/IObject.h"
#include "lib/interface_header/uuid.h"
#include "lib/interface_header/IClassModule.h"

namespace zq{


class IKernelModule : public IModule
{

public:

	virtual bool registerCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb) = 0;
	virtual bool registerCommonPropertyEvent(PROPERTY_EVENT_FUNCTOR&& cb) = 0;
	virtual bool registerCommonRecordEvent(RECORD_EVENT_FUNCTOR&& cb) = 0;

	virtual bool registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb) = 0;
	virtual bool registerClassPropertyEvent(const std::string& strClassName, PROPERTY_EVENT_FUNCTOR&& cb) = 0;
	virtual bool registerClassRecordEvent(const std::string& strClassName, RECORD_EVENT_FUNCTOR&& cb) = 0;

	virtual std::shared_ptr<IObject> getObject(const Guid& ident) = 0;
	virtual std::shared_ptr<IObject> createObject(const Guid& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const DataList& arg) = 0;

	virtual bool destroySelf(const Guid& self) = 0;
	virtual bool destroyObject(const Guid& self) = 0;
	virtual bool destroyAll() = 0;

	virtual bool findProperty(const Guid& self, const std::string& strPropertyName) = 0;

	virtual bool setPropertyInt(const Guid& self, const std::string& strPropertyName, const int64 nValue) = 0;
	virtual bool setPropertyFloat(const Guid& self, const std::string& strPropertyName, const double dValue) = 0;
	virtual bool setPropertyString(const Guid& self, const std::string& strPropertyName, const std::string& strValue) = 0;
	virtual bool setPropertyObject(const Guid& self, const std::string& strPropertyName, const Guid& objectValue) = 0;

	virtual int64 getPropertyInt(const Guid& self, const std::string& strPropertyName) = 0;
	virtual double getPropertyFloat(const Guid& self, const std::string& strPropertyName) = 0;
	virtual const std::string& getPropertyString(const Guid& self, const std::string& strPropertyName) = 0;
	virtual const Guid& getPropertyObject(const Guid& self, const std::string& strPropertyName) = 0;

	virtual std::shared_ptr<IRecord> findRecord(const Guid& self, const std::string& strRecordName) = 0;
	virtual bool clearRecord(const Guid& self, const std::string& strRecordName) = 0;

	virtual bool setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue) = 0;
	virtual bool setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
	virtual bool setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
	virtual bool setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const Guid& objectValue) = 0;

	virtual bool setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value) = 0;
	virtual bool setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
	virtual bool setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
	virtual bool setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value) = 0;

	virtual int64 getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
	inline int GetRecordInt32(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol) { return (int)getRecordInt(self, strRecordName, nRow, nCol); };
	virtual double getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const std::string& getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const Guid& getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;

	virtual int64 getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	inline int GetRecordInt32(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) { return (int)getRecordInt(self, strRecordName, nRow, strColTag); };
	virtual double getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const std::string& getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const Guid& getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

	virtual Guid createGUID() = 0;

	virtual int getMaxOnLineCount() = 0;

	virtual int getObjectByProperty(const int nSceneID, const int nGroupID, const std::string& strPropertyName, const DataList& valueArg, DataList& list) = 0;

	virtual void random(int nStart, int nEnd, int nCount, DataList& valueList) = 0;
	virtual int random(int nStart, int nEnd) = 0;
	virtual float random() = 0;
	virtual bool info(const Guid ident) = 0;
};

}
