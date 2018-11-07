#include "httpserver_module.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libboost_system-vc141-mt-gd-x64-1_67.lib")
#pragma comment(lib, "libboost_date_time-vc141-mt-gd-x64-1_67.lib")	
#pragma comment(lib, "libboost_regex-vc141-mt-gd-x64-1_67.lib")
#endif

using namespace zq;

HttpServerModule::HttpServerModule(ILibManager* p)
{
    libManager_ = p;
}

HttpServerModule::~HttpServerModule()
{
}

bool HttpServerModule::initEnd()
{
	server_.set_http_error_handler(std::bind(&HttpServerModule::onErrorRequest, this, std::placeholders::_1, std::placeholders::_2));
	return true;
}

bool HttpServerModule::run()
{
	server_.poll();
    return true;
}

bool HttpServerModule::shut()
{
	server_.stop();
	return true;
}

int HttpServerModule::startServer(uint16 port)
{
#ifdef ZQ_ENABLE_SSL
	server.init_ssl_context(true, [](auto, auto) { return "123456"; }, "server.crt", "server.key", "dh1024.pem");
	bool r = server_.listen("0.0.0.0", std::to_string(port);
#else
	bool r = server_.listen("0.0.0.0", std::to_string(port));
#endif
	if (!r)
	{
		LOG_INFO << "listen failed, port: " << port;
		return -1;
	}

	return 0;
}

void HttpServerModule::setHandler(http_method method, const std::string& path, HttpServerHandler&& fun)
{
	server_.set_http_handler(method, path, std::move(fun));
}

void HttpServerModule::onErrorRequest(request& req, response& res)
{
	res.set_status_and_content(status_type::bad_request, "the path is not right");
}
