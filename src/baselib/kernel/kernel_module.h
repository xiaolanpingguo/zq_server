#pragma once


#include "baselib/core/IObject.h"
#include "baselib/core/data_list.hpp"
#include "interface_header/base/uuid.h"
#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"

#include <unordered_map>

namespace zq {


class KernelModule : public IKernelModule
{
public:
	KernelModule(ILibManager* p);
	virtual ~KernelModule();

	bool init() override;
	bool initEnd() override;
	bool shut() override;
	bool run() override;

	Guid createGUID() override;
	bool existObject(const Guid& ident) override;
	std::shared_ptr<IObject> getObject(const Guid& ident) override;
	std::shared_ptr<IObject> createObject(const std::string& class_name) override;
	bool destroyObject(const Guid& self) override;

protected:

	bool destroyAll();

	void initRandom();
	int onPropertyCommonEvent(const Guid& self, const std::string& strPropertyName, const VariantData& oldVar, const VariantData& newVar);

private:
	std::vector<float> vecRandom_;
	int guidIndex_;
	int randomPos_;

	Guid curExeObject_;
	uint64 lastActTime_;

	IClassModule* classModule_;
};

}
