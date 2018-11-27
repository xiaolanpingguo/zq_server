#pragma once


#include <list>
#include "baselib/core/property_manager.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IKernelModule.h"

namespace zq {

class CClass;
class CClassModule;
using CClassPtr = std::shared_ptr<CClass>;
class CClass : public IClass
{
	friend class CClassModule;

	struct ClassPropertyDescInfo
	{
		std::string desc_name;
		std::string desc_value;
	};
public:

	CClass(const std::string& strClassName) : strClassName_(strClassName)
	{
	}

	virtual ~CClass()
	{
		memObjs_.clear();
		staticObjs_.clear();
	}

	const std::string& getClassName()
	{
		return strClassName_;
	}

	PropertyManager& getDescPropertyMgr()
	{
		return propertyDescMgr_;
	}

	bool addStaticObj(std::string& obj_name, IObjectPtr obj)
	{
		if (obj == nullptr)
		{
			return false;
		}

		return staticObjs_.insert(std::make_pair(obj_name, obj)).second;
	}

	bool addMemObj(IObjectPtr obj)
	{
		if (obj == nullptr)
		{
			return false;
		}

		return memObjs_.insert(std::make_pair(obj->getGuid(), obj)).second;
	}

	IObjectPtr getMemObj(const Guid& id)
	{
		auto it = memObjs_.find(id);
		if (it == memObjs_.end())
		{
			return nullptr;
		}

		return it->second;
	}

	bool removeMemObj(const Guid& id)
	{
		auto it = memObjs_.find(id);
		if (it != memObjs_.end())
		{
			memObjs_.erase(it);
			return true;
		}

		return false;
	}

	const AllStaticObjListT& getAllStaticObjs()
	{
		return staticObjs_;
	}

	const AllMemObjListT& getAllMemObjs()
	{
		return memObjs_;
	}

	bool existMemberClassName(const std::string& name)
	{
		for (const auto& ele : vecMemberClassName_)
		{
			if (ele == name)
			{
				return true;
			}
		}

		return false;
	}

	void addMemberClassName(const std::string& name)
	{
		if (!existMemberClassName(name))
		{
			vecMemberClassName_.push_back(name);
		}
	}

private:

	PropertyManager propertyDescMgr_;

	std::string strType_;
	std::string strClassName_;

	AllMemObjListT memObjs_;
	AllStaticObjListT staticObjs_;

	// 该类的成员变量
	std::vector<std::string> vecMemberClassName_;
};

class CClassModule : public IClassModule
{
	using AllClassListT = std::unordered_map<std::string, CClassPtr>;
public:
	CClassModule(ILibManager* p);
	virtual ~CClassModule();

	bool init() override;
	bool shut() override;
	bool load() override;
;
protected:

	IClassPtr getClass(const std::string& class_anme) override;
	CClassPtr findClass(const std::string& class_anme);

	bool readXml(const std::string& file_name);
	bool readDBXml(const std::string& file_name);
	bool addClass(const std::string& strClassName, CClassPtr newclass);

	bool existObject(const std::string& class_name, const Guid& ident) override;
	IObjectPtr getObject(const std::string& class_name, const Guid& ident) override;
	IObjectPtr createObject(const std::string& class_name) override;
	bool destroyObject(const std::string& class_name, const Guid& self) override;

protected:

	std::string strXmlDir_;
	std::string strDBXmlDir_;
	AllClassListT classList_;

	IKernelModule* kernalModule_;
};				   

}
