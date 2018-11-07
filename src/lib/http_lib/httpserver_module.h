#pragma once 


#include "lib/interface_header/IHttpServerModule.h"
#include "server/server.hpp"

namespace zq {

class HttpServerModule : public IHttpServerModule
{
public:
	HttpServerModule(ILibManager* p);
	virtual ~HttpServerModule();

public:

	bool initEnd() override;
	bool run() override;
	bool shut() override;

	int startServer(uint16 port) override;

	void setHandler(http_method method, const std::string& path, HttpServerHandler&& fun) override;

private:

	void onErrorRequest(request& req, response& res);

private:

	http_server_single_thread server_;
};

}

