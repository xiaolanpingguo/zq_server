#pragma once

#include "element_module.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "baselib/core/map_ex.hpp"
#include "baselib/core/list.hpp"
#include "baselib/core/data_list.hpp"
#include "baselib/core/record.h"
#include "baselib/core/property_manager.h"
#include "baselib/core/record_manager.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IElementModule.h"
#include "interface_header/base/ILibManager.h"

namespace zq {

class CClass : public IClass
{
public:

	CClass(const std::string& strClassName)
	{
		parentClass_ = nullptr;
		strClassName_ = strClassName;

		propertyManager_ = std::shared_ptr<IPropertyManager>(new PropertyManager(Guid()));
		recordManager_ = std::shared_ptr<IRecordManager>(new RecordManager(Guid()));
	}

	virtual ~CClass()
	{
		clearAll();
	}

	virtual std::shared_ptr<IPropertyManager> getPropertyManager()
	{
		return propertyManager_;
	}

	virtual std::shared_ptr<IRecordManager> getRecordManager()
	{
		return recordManager_;
	}

	virtual bool registerClassCallBack(CLASS_EVENT_FUNCTOR&& cb)
	{
		classEventCb_.emplace_back(std::move(cb));
		return true;
	}

	virtual bool doEvent(const Guid& objectID, const CLASS_OBJECT_EVENT eClassEvent, const DataList& valueList)
	{
		for (size_t i = 0; i < classEventCb_.size(); ++i)
		{
			(classEventCb_[i])(objectID, strClassName_, eClassEvent, valueList);
		}

		return true;
	}

	void setParent(std::shared_ptr<IClass> pClass)
	{
		parentClass_ = pClass;
	}

	std::shared_ptr<IClass> getParent()
	{
		return parentClass_;
	}

	void setTypeName(const char* strType)
	{
		strType_ = strType;
	}

	const std::string& getTypeName()
	{
		return strType_;
	}

	const std::string& getClassName()
	{
		return strClassName_;
	}

	const bool addId(std::string& strId)
	{
		IdList_.push_back(strId);
		return true;
	}

	const std::vector<std::string>& getIDList()
	{
		return IdList_;
	}

	void clearIdList()
	{
		IdList_.clear();
	}

	void setInstancePath(const std::string& strPath)
	{
		strClassInstancePath_ = strPath;
	}

	const std::string& getInstancePath()
	{
		return strClassInstancePath_;
	}

private:
	std::shared_ptr<IPropertyManager> propertyManager_;
	std::shared_ptr<IRecordManager> recordManager_;

	std::shared_ptr<IClass> parentClass_;
	std::string strType_;
	std::string strClassName_;
	std::string strClassInstancePath_;

	std::vector<std::string> IdList_;
	std::vector<CLASS_EVENT_FUNCTOR> classEventCb_;
};

class CClassModule : public IClassModule
{
public:
	CClassModule(ILibManager* p);
	virtual ~CClassModule();

	virtual bool init();
	virtual bool shut();

	virtual bool load();
	virtual bool Save();
	virtual bool clear();

	virtual bool registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb);
	virtual bool doEvent(const Guid& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const DataList& valueList);

	virtual std::shared_ptr<IPropertyManager> getClassPropertyManager(const std::string& strClassName);
	virtual std::shared_ptr<IRecordManager> getClassRecordManager(const std::string& strClassName);

	virtual bool addClass(const std::string& strClassName, const std::string& strParentName);

protected:
	virtual EN_DATA_TYPE computerType(const char* pstrTypeName, AbstractData& var);
	virtual bool addPropertys(rapidxml::xml_node<>* pPropertyRootNode, std::shared_ptr<IClass> pClass);
	virtual bool addRecords(rapidxml::xml_node<>* pRecordRootNode, std::shared_ptr<IClass> pClass);
	virtual bool addClassInclude(const char* pstrClassFilePath, std::shared_ptr<IClass> pClass);
	virtual bool addClass(const char* pstrClassFilePath, std::shared_ptr<IClass> pClass);

	virtual bool load(rapidxml::xml_node<>* attrNode, std::shared_ptr<IClass> pParentClass);

protected:
	IElementModule * elementModule_;

	std::string logicClassConfigName_;
	std::string cfgRootPath_;
	std::string structPath_;
	std::string implPath_;
};

}
