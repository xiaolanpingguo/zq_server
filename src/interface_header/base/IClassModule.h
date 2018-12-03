#pragma once


#include "IModule.h"
#include "baselib/core/IObject.h"
#include <unordered_map>

namespace zq{


using AllObjListT = std::map<uuid, IObjectPtr>;
class IClass 
{
public:

	virtual const std::string& getClassName() = 0;
	virtual IObjectPtr getObject(const uuid& id) = 0;
	virtual const AllObjListT& getAllObjs() = 0;
};


using IClassPtr = std::shared_ptr<IClass>;
class IClassModule : public IModule
{
public:
	virtual ~IClassModule() {}

	virtual IClassPtr getClass(const std::string& class_anme) = 0;

	virtual bool existObject(const std::string& class_name, const uuid& ident) = 0;
	virtual IObjectPtr getObject(const std::string& class_name, const uuid& ident) = 0;
	virtual IObjectPtr createObject(const std::string& strClassName) = 0;
	virtual bool destroyObject(const std::string& class_name, const uuid& ident) = 0;
};

}