//#pragma once
//
//#include "baselib/core/IObject.h"
//#include "interface_header/base/uuid.h"
//#include "interface_header/base/IEventModule.h"
//#include "interface_header/base/IKernelModule.h"
//
//namespace zq {
//
//class EventModule : public IEventModule
//{
//public:
//	EventModule(ILibManager* p)
//	{
//		libManager_ = p;
//	}
//
//
//	virtual ~EventModule()
//	{
//	}
//
//	virtual bool init();
//	virtual bool initEnd();
//	virtual bool beforeShut();
//	virtual bool shut();
//	virtual bool run();
//
//	virtual bool doEvent(const EnEventDefine nEventID, const DataList& valueList);
//
//	virtual bool existEventCallBack(const EnEventDefine nEventID);
//	virtual bool removeEventCallBack(const EnEventDefine nEventID);
//
//	//////////////////////////////////////////////////////////
//	virtual bool doEvent(const uuid self, const EnEventDefine nEventID, const DataList& valueList);
//
//	virtual bool existEventCallBack(const uuid self, const EnEventDefine nEventID);
//
//	virtual bool removeEventCallBack(const uuid self, const EnEventDefine nEventID);
//	virtual bool removeEventCallBack(const uuid self);
//
//protected:
//
//	virtual bool addEventCallBack(const EnEventDefine nEventID, const MODULE_EVENT_FUNCTOR_PTR cb);
//	virtual bool addEventCallBack(const uuid self, const EnEventDefine nEventID, const OBJECT_EVENT_FUNCTOR_PTR cb);
//
//private:
//
//	IKernelModule * m_pKernelodule;
//
//private:
//	// for module
//	List<EnEventDefine> mModuleRemoveListEx;
//	MapEx<EnEventDefine, List<MODULE_EVENT_FUNCTOR_PTR>> mModuleEventInfoMapEx;
//
//	//for object
//	List<uuid> mObjectRemoveListEx;
//	MapEx<uuid, MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>> mObjectEventInfoMapEx;
//};
//
//}