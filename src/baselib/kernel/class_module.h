#pragma once


#include <list>
#include <random>
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
public:

	CClass(const std::string& strClassName);
	~CClass();

	bool existObject(const uuid& id);
	IObjectPtr addObject(const uuid& id);
	IObjectPtr getObject(const uuid& id);
	bool destroyObject(const uuid& id);
	AllObjListT& getAllObjs() override { return allObjs_; }

private:

	std::string strClassName_;
	AllObjListT allObjs_;
};


class CClassModule : public IClassModule
{
	using AllClassListT = std::unordered_map<std::string, CClassPtr>;
public:
	CClassModule(ILibManager* p);
	virtual ~CClassModule();

	bool init() override;
	bool initEnd() override;
	bool shut() override;
	bool run() override;
;
protected:

	IClassPtr getClass(const std::string& class_anme) override;
	CClassPtr findClass(const std::string& class_anme);

	bool addClass(const std::string& strClassName, CClassPtr newclass);

	bool existObject(const std::string& class_name, const uuid& ident) override;
	IObjectPtr getObject(const std::string& class_name, const uuid& ident) override;
	IObjectPtr createObject(const std::string& class_name) override;
	bool destroyObject(const std::string& class_name, const uuid& self) override;

protected:

	std::string strXmlDir_;
	AllClassListT classList_;

	IKernelModule* kernalModule_;
};				   

}
