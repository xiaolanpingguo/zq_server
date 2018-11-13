//#include "event_module.h"
//using namespace zq;
//
//bool EventModule::init()
//{
//	m_pKernelodule = getLibManager()->findModule<IKernelModule>();
//
//    return true;
//}
//
//bool EventModule::initEnd()
//{
//    return true;
//}
//
//bool EventModule::beforeShut()
//{	
//	mModuleEventInfoMapEx.clear();
//	mObjectEventInfoMapEx.clear();
//
//    return true;
//}
//
//bool EventModule::shut()
//{
//    return true;
//}
//
//bool EventModule::run()
//{
//	//remove
//	if (mModuleRemoveListEx.count() > 0)
//	{
//		EnEventDefine nEventID = EnEventDefine::EN_UNKNOW;;
//		bool bRet = mModuleRemoveListEx.first(nEventID);
//		while (bRet)
//		{
//			mModuleEventInfoMapEx.removeElement(nEventID);
//
//			bRet = mModuleRemoveListEx.next(nEventID);
//		}
//	}
//
//	//////
//	if (mObjectRemoveListEx.count() > 0)
//	{
//		Guid xObject;
//		bool bRet = mObjectRemoveListEx.first(xObject);
//		while (bRet)
//		{
//			mObjectEventInfoMapEx.removeElement(xObject);
//
//			bRet = mObjectRemoveListEx.next(xObject);
//		}
//	}
//
//    return true;
//
//}
//
//bool EventModule::doEvent(const EnEventDefine nEventID, const DataList & valueList)
//{
//	bool bRet = false;
//
//	std::shared_ptr<List<MODULE_EVENT_FUNCTOR_PTR>> xEventListPtr = mModuleEventInfoMapEx.getElement(nEventID);
//	if (xEventListPtr)
//	{
//		MODULE_EVENT_FUNCTOR_PTR pFunPtr;
//		bool bRet = xEventListPtr->first(pFunPtr);
//		while (bRet)
//		{
//			MODULE_EVENT_FUNCTOR* pFunc = pFunPtr.get();
//			pFunc->operator()(nEventID, valueList);
//
//			bRet = xEventListPtr->next(pFunPtr);
//		}
//
//		bRet = true;
//	}
//
//	return bRet;
//}
//
//bool EventModule::existEventCallBack(const EnEventDefine nEventID)
//{
//	return mModuleEventInfoMapEx.existElement(nEventID);
//}
//
//bool EventModule::removeEventCallBack(const EnEventDefine nEventID)
//{
//	return mModuleEventInfoMapEx.removeElement(nEventID);
//}
//
//bool EventModule::doEvent(const Guid self, const EnEventDefine nEventID, const DataList & valueList)
//{
//	bool bRet = false;
//
//	//if (!m_pKernelodule->existObject(self))
//	//{
//	//	return bRet;
//	//}
//
//	std::shared_ptr<MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>> xEventMapPtr = mObjectEventInfoMapEx.getElement(self);
//	if (!xEventMapPtr)
//	{
//		return bRet;
//	}
//
//	std::shared_ptr<List<OBJECT_EVENT_FUNCTOR_PTR>> xEventListPtr = xEventMapPtr->getElement(nEventID);
//	if (!xEventListPtr)
//	{
//		return bRet;
//	}
//
//	OBJECT_EVENT_FUNCTOR_PTR pFunPtr;
//	bool bFunRet = xEventListPtr->first(pFunPtr);
//	while (bFunRet)
//	{
//		OBJECT_EVENT_FUNCTOR* pFunc = pFunPtr.get();
//		pFunc->operator()(self, nEventID, valueList);
//
//		bFunRet = xEventListPtr->next(pFunPtr);
//	}
//
//	return bRet;
//}
//
//bool EventModule::existEventCallBack(const Guid self, const EnEventDefine nEventID)
//{
//	std::shared_ptr<MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>> xEventMapPtr = mObjectEventInfoMapEx.getElement(self);
//	if (!xEventMapPtr)
//	{
//		return false;
//	}
//
//	return xEventMapPtr->existElement(nEventID);
//}
//
//bool EventModule::removeEventCallBack(const Guid self, const EnEventDefine nEventID)
//{
//	std::shared_ptr<MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>> xEventMapPtr = mObjectEventInfoMapEx.getElement(self);
//	if (!xEventMapPtr)
//	{
//		return false;
//	}
//
//	return xEventMapPtr->removeElement(nEventID);
//}
//
//bool EventModule::removeEventCallBack(const Guid self)
//{
//	return mObjectEventInfoMapEx.removeElement(self);
//}
//
//bool EventModule::addEventCallBack(const EnEventDefine nEventID, const MODULE_EVENT_FUNCTOR_PTR cb)
//{
//	std::shared_ptr<List<MODULE_EVENT_FUNCTOR_PTR>> xEventListPtr = mModuleEventInfoMapEx.getElement(nEventID);
//	if (!xEventListPtr)
//	{
//		xEventListPtr = std::shared_ptr<List<MODULE_EVENT_FUNCTOR_PTR>>(new List<MODULE_EVENT_FUNCTOR_PTR>());
//		mModuleEventInfoMapEx.addElement(nEventID, xEventListPtr);
//	}
//
//	xEventListPtr->add(cb);
//
//	return false;
//}
//
//bool EventModule::addEventCallBack(const Guid self, const EnEventDefine nEventID, const OBJECT_EVENT_FUNCTOR_PTR cb)
//{
//	//if (!m_pKernelodule->existObject(self))
//	//{
//	//	return false;
//	//}
//	
//	std::shared_ptr<MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>> xEventMapPtr = mObjectEventInfoMapEx.getElement(self);
//	if (!xEventMapPtr)
//	{
//		xEventMapPtr = std::shared_ptr<MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>>(new MapEx<EnEventDefine, List<OBJECT_EVENT_FUNCTOR_PTR>>());
//		mObjectEventInfoMapEx.addElement(self, xEventMapPtr);
//	}
//
//	std::shared_ptr<List<OBJECT_EVENT_FUNCTOR_PTR>> xEventListPtr =  xEventMapPtr->getElement(nEventID);
//	if (!xEventListPtr)
//	{
//		xEventListPtr = std::shared_ptr<List<OBJECT_EVENT_FUNCTOR_PTR>>(new List<OBJECT_EVENT_FUNCTOR_PTR>());
//		xEventMapPtr->addElement(nEventID, xEventListPtr);
//	}
//
//	xEventListPtr->add(cb);
//
//	return true;
//}
