#pragma once


#include "IModule.h"

namespace zq {


enum EnEventDefine
{
	EN_UNKNOW = 0,
	////////////////Has self---logic//////////////////////////////////////////////////////////
	//arg[0]:resultID[EGameErrorCode]
	EN_ON_GENERAL_MESSAGE,

	////////////////core events for NF--start////////////////////////////////////////////////////////
	//scene swap
	//common property
	//common record

	////////////////core events for NF--end//////////////////////////////////////////////////////////
	//arg[0]:x,y,z
	EN_ON_CLIENT_REQUIRE_MOVE,
	EN_ON_CLIENT_MOVE_RESULT,

	//arg[0]:itemID,TargetObject
	EN_ON_CLIENT_REQUIRE_USE_ITEM,

	//arg[0]:itemID, fx, fy, fz, TargetObjectList
	EN_ON_CLIENT_REQUIRE_USE_ITEM_POS,


	EN_ON_NOTICE_ECTYPE_AWARD,

};

class IEventModule
	: public IModule
{
public:
protected:

	typedef std::function<int(const Guid&, const EnEventDefine, const DataList&)> OBJECT_EVENT_FUNCTOR;
	typedef std::function<int(const EnEventDefine, const DataList&)> MODULE_EVENT_FUNCTOR;

	typedef std::shared_ptr<OBJECT_EVENT_FUNCTOR> OBJECT_EVENT_FUNCTOR_PTR;//EVENT
	typedef std::shared_ptr<MODULE_EVENT_FUNCTOR> MODULE_EVENT_FUNCTOR_PTR;//EVENT

public:

	virtual bool doEvent(const EnEventDefine nEventID, const DataList& valueList) = 0;

	virtual bool existEventCallBack(const EnEventDefine nEventID) = 0;

	virtual bool removeEventCallBack(const EnEventDefine nEventID) = 0;

	template<typename BaseType>
	bool addEventCallBack(const EnEventDefine nEventID, BaseType* pBase, int (BaseType::*handler)(const EnEventDefine, const DataList&))
	{
		MODULE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
		MODULE_EVENT_FUNCTOR_PTR functorPtr(new MODULE_EVENT_FUNCTOR(functor));
		return addEventCallBack(nEventID, functorPtr);
	}

	virtual bool doEvent(const Guid self, const EnEventDefine nEventID, const DataList& valueList) = 0;

	virtual bool existEventCallBack(const Guid self, const EnEventDefine nEventID) = 0;

	virtual bool removeEventCallBack(const Guid self, const EnEventDefine nEventID) = 0;
	virtual bool removeEventCallBack(const Guid self) = 0;

	template<typename BaseType>
	bool addEventCallBack(const Guid& self, const EnEventDefine nEventID, BaseType* pBase, int (BaseType::*handler)(const Guid&, const EnEventDefine, const DataList&))
	{
		OBJECT_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		OBJECT_EVENT_FUNCTOR_PTR functorPtr(new OBJECT_EVENT_FUNCTOR(functor));
		return addEventCallBack(self, nEventID, functorPtr);
	}

protected:

	virtual bool addEventCallBack(const EnEventDefine nEventID, const MODULE_EVENT_FUNCTOR_PTR cb) = 0;
	virtual bool addEventCallBack(const Guid self, const EnEventDefine nEventID, const OBJECT_EVENT_FUNCTOR_PTR cb) = 0;

};

}