#include "test_dll_module.h"
#include "interface_header/base/IHttpServerModule.h"
#include "interface_header/base/IConfigModule.h"
#include "config_header/cfg_server.hpp"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#endif

using namespace zq;

TestDllModule::TestDllModule(ILibManager* p)
{
    libManager_ = p;
}

TestDllModule::~TestDllModule()
{
}

bool zq::TestDllModule::init()
{
	return true;
}

bool TestDllModule::initEnd()
{
	auto httpServerModule = libManager_->findModule<IHttpServerModule>();
	httpServerModule->setHandler(http_method::POST, "/print", [this](request& req, response& res) {
		this->print(req,res);
	});

	auto configModule = libManager_->findModule<IConfigModule>();
	auto csvServer = configModule->getCsvRow<CSVServer>(9);
	httpServerModule->startServer(csvServer->http_port);
	return true;
}

bool TestDllModule::run()
{
    return true;
}

bool TestDllModule::shut()
{
	auto httpServerModule = libManager_->findModule<IHttpServerModule>();
	httpServerModule->setHandler(http_method::POST, "/print", HttpServerHandler());
	return true;
}

void zq::TestDllModule::print(request& req,response& res)
{
	if (req.has_body())
	{
		auto reqBody = req.body();
		int baseNum = std::stoi(reqBody);
		
		res.set_status_and_content(status_type::ok, std::move(std::to_string(baseNum * 5)));
	}
	else
	{
		res.set_status_and_content(status_type::bad_request,std::move(""));
	}
	
	return;
}
