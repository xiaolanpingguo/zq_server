#pragma once


#include "IModule.h"
#include "uuid.h"

namespace zq{


class IKernelModule : public IModule
{
public:

	virtual uuid gen_uuid() = 0;
};

}
