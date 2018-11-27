#pragma once


#include <functional>
#include "IModule.h"
#include "baselib/core/IObject.h"
#include "uuid.h"
#include "IClassModule.h"

namespace zq{


class IKernelModule : public IModule
{
public:

	virtual Guid createGUID() = 0;

	virtual bool existObject(const Guid& ident) = 0;
	virtual std::shared_ptr<IObject> getObject(const Guid& ident) = 0;
	virtual std::shared_ptr<IObject> createObject(const std::string& strClassName) = 0;
	virtual bool destroyObject(const Guid& self) = 0;
};

}
