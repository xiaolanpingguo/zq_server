#include "class_module.h"
#include "baselib/base_code/util.h"
#include "baselib/core/object.h"
#include "dependencies/RapidXML/rapidxml.hpp"
#include "dependencies/RapidXML/rapidxml_print.hpp"


namespace zq{


CClass::CClass(const std::string& strClassName) : strClassName_(strClassName)
{
}

CClass::~CClass()
{
}

bool CClass::existObject(const uuid& id)
{
	return allObjs_.find(id) != allObjs_.end();
}

IObjectPtr CClass::addObject(const uuid& id)
{
	if (!existObject(id))
	{
		return false;
	}

	IObjectPtr obj = std::make_shared<CObject>(id);
	allObjs_[id] = obj;
	return obj;
}

IObjectPtr CClass::getObject(const uuid& id)
{
	auto it = allObjs_.find(id);
	if (it == allObjs_.end())
	{
		return nullptr;
	}

	return it->second;
}

bool CClass::destroyObject(const uuid& id)
{
	auto it = allObjs_.find(id);
	if (it == allObjs_.end())
	{
		return false;
	}

	allObjs_.erase(it);
	return true;
}




/*----------------------------------------------------*/

CClassModule::CClassModule(ILibManager* p)
{
    libManager_ = p;
	strXmlDir_ = libManager_->getCfgXmlDir();
}

CClassModule::~CClassModule()
{
}

bool CClassModule::init()
{
	kernalModule_ = libManager_->findModule<IKernelModule>();
	return true;
}

bool CClassModule::initEnd()
{
	return true;
}

bool CClassModule::shut()
{
	return true;
}

bool CClassModule::run()
{
	return true;
}

bool CClassModule::addClass(const std::string& strClassName, CClassPtr newclass)
{
	return classList_.insert(std::make_pair(strClassName, newclass)).second;
}

std::shared_ptr<IClass> CClassModule::getClass(const std::string& class_anme)
{
	auto it = classList_.find(class_anme);
	if (it == classList_.end())
	{
		return nullptr;
	}

	return it->second;
}

CClassPtr CClassModule::findClass(const std::string& class_anme)
{
	auto it = classList_.find(class_anme);
	if (it == classList_.end())
	{
		return nullptr;
	}

	return it->second;
}

IObjectPtr CClassModule::createObject(const std::string& class_name)
{
	using namespace std::placeholders;

	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return nullptr;
	}

	// 这里不可能存在，不然这个函数就出大问题了
	uuid ident = kernalModule_->gen_uuid();
	if (cclass->existObject(ident))
	{
		LOG_ERROR << "The object has Exists, id: " << ident;
		return nullptr;
	}

	return cclass->addObject(ident);
}

bool CClassModule::destroyObject(const std::string& class_name, const uuid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass == nullptr, class_name: " << class_name;
		return false;
	}

	return cclass->destroyObject(ident);
}

IObjectPtr CClassModule::getObject(const std::string& class_name, const uuid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return nullptr;
	}

	return cclass->getObject(ident);
}

bool CClassModule::existObject(const std::string& class_name, const uuid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return false;
	}

	return cclass->existObject(ident);
}


}
