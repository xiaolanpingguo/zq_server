#pragma once


#include "IModule.h"
#include "lib/core/list.hpp"
#include "lib/core/IObject.h"

namespace zq{


using CLASS_EVENT_FUNCTOR = std::function<int(const Guid&, const std::string&, const CLASS_OBJECT_EVENT, const DataList&)>;
class IClass : public List<std::string>
{
public:
	virtual ~IClass() {}

	virtual std::shared_ptr<IPropertyManager> getPropertyManager() = 0;
	virtual std::shared_ptr<IRecordManager> getRecordManager() = 0;

	virtual void setParent(std::shared_ptr<IClass> pClass) = 0;
	virtual std::shared_ptr<IClass> getParent() = 0;
	virtual void setTypeName(const char* strType) = 0;
	virtual const std::string& getTypeName() = 0;
	virtual const std::string& getClassName() = 0;
	virtual const bool addId(std::string& strConfigName) = 0;
	virtual const std::vector<std::string>& getIDList() = 0;
	virtual void clearIdList() = 0;
	virtual const std::string& getInstancePath() = 0;
	virtual void setInstancePath(const std::string& strPath) = 0;

	virtual bool registerClassCallBack(CLASS_EVENT_FUNCTOR&& cb) = 0;
	virtual bool doEvent(const Guid& objectID, const CLASS_OBJECT_EVENT eClassEvent, const DataList& valueList) = 0;
};

class IClassModule : public IModule, public MapEx<std::string, IClass>
{
public:
	virtual ~IClassModule() {}
	virtual bool load() = 0;
	virtual bool Save() = 0;
	virtual bool clear() = 0;

	virtual bool doEvent(const Guid& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const DataList& valueList) = 0;

	virtual bool registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb) = 0;

	virtual std::shared_ptr<IPropertyManager> getClassPropertyManager(const std::string& strClassName) = 0;

	virtual std::shared_ptr<IRecordManager> getClassRecordManager(const std::string& strClassName) = 0;

};

}