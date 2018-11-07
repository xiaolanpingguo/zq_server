#pragma once

#include "IModule.h"
#include "baselib/core/data_list.hpp"
#include "baselib/core/IPropertyManager.h"
#include "baselib/core/IRecordManager.h"

namespace zq {

class IElementModule : public IModule
{
public:
	virtual bool load() = 0;
	virtual bool save() = 0;
	virtual bool reload(const std::string& name) = 0;

	//special
	virtual bool loadSceneInfo(const std::string& strFileName, const std::string& strClassName) = 0;

	virtual bool existElement(const std::string& strConfigName) = 0;
	virtual bool existElement(const std::string& strClassName, const std::string& strConfigName) = 0;

	virtual std::shared_ptr<IPropertyManager> getPropertyManager(const std::string& strConfigName) = 0;
	virtual std::shared_ptr<IRecordManager> getRecordManager(const std::string& strConfigName) = 0;

	virtual int64 getPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;
	virtual double getPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName) = 0;
	virtual const std::string& getPropertyString(const std::string& strConfigName, const std::string& strPropertyName) = 0;

	virtual const std::vector<std::string> getListByProperty(const std::string& strClassName, const std::string& strPropertyName, const int64 nValue) = 0;
	virtual const std::vector<std::string> getListByProperty(const std::string& strClassName, const std::string& strPropertyName, const std::string& nValue) = 0;

};

}
