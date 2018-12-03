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
class IObject : public MemoryCounter<IObject>
{
public:
	IObject() : MemoryCounter(GET_CLASS_NAME(IObject))
	{
	}

	virtual ~IObject() = default;

	virtual const uuid& getGuid() = 0;
	virtual const std::string& getObjName() = 0;
	virtual const std::string& getClassName() = 0;

	virtual bool exsitProperty(const std::string& strPropertyName) = 0;
	virtual IPropertyPtr getProperty(const std::string& strPropertyName) = 0;
	virtual bool addProperty(IPropertyPtr prop) = 0;

	//virtual bool exsitArrayProperty(const std::string& strPropertyName) = 0;
	//virtual PropertyArrayPtr getArrayProperty(const std::string& strPropertyName) = 0;
	//virtual bool addArrayProperty(PropertyArrayPtr prop) = 0;
};

}
