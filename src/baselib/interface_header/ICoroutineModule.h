#pragma once

#include <stdint.h>
#include <type_traits>
#include <functional>
#include "baselib/interface_header/platform.h"
#include "baselib/interface_header/IModule.h"

namespace zq {

using TaskFunction = std::function<void()>;

class ICoroutineModule : public IModule
{
public:
};

}
























#endif
