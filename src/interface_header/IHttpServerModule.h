#pragma once

#include "IModule.h"

#include "baselib/http_lib/server/request.hpp"

namespace zq {

using HttpServerHandler = std::function<void(request&, response&)>;
class IHttpServerModule : public IModule
{
public:

	virtual int startServer(uint16 port) = 0;

	virtual void setHandler(http_method method, const std::string& path, HttpServerHandler&& fun) = 0;
};

}

