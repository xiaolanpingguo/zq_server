#pragma once

#include <map>
#include <string>
#include <iostream>
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "Dependencies/RapidXML/rapidxml_iterators.hpp"
#include "Dependencies/RapidXML/rapidxml_print.hpp"
#include "Dependencies/RapidXML/rapidxml_utils.hpp"
#include "lib/core/map_ex.hpp"
#include "lib/core/list.hpp"
#include "lib/core/data_list.hpp"
#include "lib/core/record.h"
#include "lib/core/property_manager.h"
#include "lib/core/record_manager.h"
#include "lib/interface_header/IElementModule.h"
#include "lib/interface_header/IClassModule.h"

namespace zq {

class CClass;

class ElementConfigInfo
{
public:
	ElementConfigInfo()
	{
		propertyManager_ = std::shared_ptr<IPropertyManager>(new PropertyManager(Guid()));
		recordManager_ = std::shared_ptr<IRecordManager>(new RecordManager(Guid()));
	}

	virtual ~ElementConfigInfo()
	{
	}

	std::shared_ptr<IPropertyManager> getPropertyManager()
	{
		return propertyManager_;
	}

	std::shared_ptr<IRecordManager> getRecordManager()
	{
		return recordManager_;
	}

protected:

	std::shared_ptr<IPropertyManager> propertyManager_;
	std::shared_ptr<IRecordManager> recordManager_;
};

class ElementModule : public IElementModule, MapEx<std::string, ElementConfigInfo>
{
public:
	ElementModule(ILibManager* p);
	virtual ~ElementModule();

	virtual bool init();
	virtual bool shut();

	virtual bool initEnd();
	virtual bool beforeShut();
	virtual bool run();
	bool reload(const std::string& filename);

	virtual bool load();
	virtual bool save();

	virtual bool loadSceneInfo(const std::string& strFileName, const std::string& strClassName);

	virtual bool existElement(const std::string& strConfigName);
	virtual bool existElement(const std::string& strClassName, const std::string& strConfigName);

	virtual std::shared_ptr<IPropertyManager> getPropertyManager(const std::string& strConfigName);
	virtual std::shared_ptr<IRecordManager> getRecordManager(const std::string& strConfigName);

	virtual int64 getPropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
	virtual double getPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
	virtual const std::string& getPropertyString(const std::string& strConfigName, const std::string& strPropertyName);

	virtual const std::vector<std::string> getListByProperty(const std::string& strClassName, const std::string& strPropertyName, const int64 nValue);
	virtual const std::vector<std::string> getListByProperty(const std::string& strClassName, const std::string& strPropertyName, const std::string& nValue);

protected:
	virtual std::shared_ptr<IProperty> GetProperty(const std::string& strConfigName, const std::string& strPropertyName);

	virtual bool load(rapidxml::xml_node<>* attrNode, std::shared_ptr<IClass> pLogicClass);
	virtual bool CheckRef();
	virtual bool LegalNumber(const char* str);

protected:
	IClassModule* classModule_;
	bool loaded_;
};

}
