#pragma once


#include "baselib/core/IObject.h"
#include "baselib/core/data_list.hpp"
#include "baselib/core/IRecord.h"
#include "baselib/interface_header/uuid.h"
#include "baselib/interface_header/ILogModule.h"
#include "baselib/interface_header/IKernelModule.h"
#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/IEventModule.h"
#include "baselib/interface_header/IElementModule.h"

namespace zq {


class KernelModule : public IKernelModule, public MapEx<Guid, IObject>
{
public:
	KernelModule(ILibManager* p);
	virtual ~KernelModule();

	virtual bool init();
	virtual bool shut();

	virtual bool beforeShut();
	virtual bool initEnd();

	virtual bool run();

	virtual bool existObject(const Guid& ident);

	virtual std::shared_ptr<IObject> getObject(const Guid& ident);
	virtual std::shared_ptr<IObject> createObject(const Guid& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const DataList& arg);

	virtual bool destroyAll();
	virtual bool destroySelf(const Guid& self);
	virtual bool destroyObject(const Guid& self);

	// 查找一个属性是否存在
	virtual bool findProperty(const Guid& self, const std::string& strPropertyName);

	// 设置属性
	virtual bool setPropertyInt(const Guid& self, const std::string& strPropertyName, const int64 nValue);
	virtual bool setPropertyFloat(const Guid& self, const std::string& strPropertyName, const double dValue);
	virtual bool setPropertyString(const Guid& self, const std::string& strPropertyName, const std::string& strValue);
	virtual bool setPropertyObject(const Guid& self, const std::string& strPropertyName, const Guid& objectValue);

	// 获得属性
	virtual int64 getPropertyInt(const Guid& self, const std::string& strPropertyName);
	virtual double getPropertyFloat(const Guid& self, const std::string& strPropertyName);
	virtual const std::string& getPropertyString(const Guid& self, const std::string& strPropertyName);
	virtual const Guid& getPropertyObject(const Guid& self, const std::string& strPropertyName);

	// record相关
	virtual std::shared_ptr<IRecord> findRecord(const Guid& self, const std::string& strRecordName);
	virtual bool clearRecord(const Guid& self, const std::string& strRecordName);

	virtual bool setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue);
	virtual bool setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const double dwValue);
	virtual bool setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue);
	virtual bool setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const Guid& objectValue);

	virtual bool setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value);
	virtual bool setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
	virtual bool setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
	virtual bool setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value);

	virtual int64 getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol);
	virtual double getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol);
	virtual const std::string& getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol);
	virtual const Guid& getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol);

	virtual int64 getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual double getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const std::string& getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const Guid& getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);

	// 获得一个guid
	virtual Guid createGUID();

	virtual int getMaxOnLineCount();

	virtual int getObjectByProperty(const int nSceneID, const int nGroupID, const std::string& strPropertyName, const DataList& valueArgArg, DataList& list);

	virtual void random(int nStart, int nEnd, int nCount, DataList& valueList);		//return [nStart, nEnd)
	virtual int random(int nStart, int nEnd);											//return [nStart, nEnd)
	virtual float random();											//return [0f, 1f)

	virtual bool logStack();
	virtual bool info(const Guid ident);
	virtual bool logSelfInfo(const Guid ident);

	virtual bool doEvent(const Guid& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const DataList& valueList);

protected:

	virtual bool registerCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb);
	virtual bool registerCommonPropertyEvent(PROPERTY_EVENT_FUNCTOR&& cb);
	virtual bool registerCommonRecordEvent(RECORD_EVENT_FUNCTOR&& cb);

	virtual bool registerClassPropertyEvent(const std::string& strClassName, PROPERTY_EVENT_FUNCTOR&& cb);
	virtual bool registerClassRecordEvent(const std::string& strClassName, RECORD_EVENT_FUNCTOR&& cb);
protected:

	virtual bool registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb);

	void initRandom();

	int onClassCommonEvent(const Guid& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const DataList& var);
	int onPropertyCommonEvent(const Guid& self, const std::string& strPropertyName, const AbstractData& oldVar, const AbstractData& newVar);
	int onRecordCommonEvent(const Guid& self, const RECORD_EVENT_DATA& xEventData, const AbstractData& oldVar, const AbstractData& newVar);

	void processMemFree();

protected:

	std::list<Guid> mtDeleteSelfList;

	std::list<CLASS_EVENT_FUNCTOR> mtCommonClassCallBackList;
	std::list<PROPERTY_EVENT_FUNCTOR> mtCommonPropertyCallBackList;
	std::list<RECORD_EVENT_FUNCTOR> mtCommonRecordCallBackList;

	std::map<std::string, std::list<PROPERTY_EVENT_FUNCTOR>> mtClassPropertyCallBackList;
	std::map<std::string, std::list<RECORD_EVENT_FUNCTOR>> mtClassRecordCallBackList;

private:
	std::vector<float> vecRandom_;
	int guidIndex_;
	int randomPos_;

	Guid curExeObject_;
	uint64 lastActTime_;

	ILogModule* logModule_;
	IClassModule* classModule_;
	IElementModule* elementModule_;
	IEventModule* m_pEventModule;
};

}
