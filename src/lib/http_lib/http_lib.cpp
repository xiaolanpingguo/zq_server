#include "http_lib.h"
#include "httpserver_module.h"
#include "httpclient_module.h"
using namespace zq;

#ifdef ZQ_DYNAMIC_PLUGIN

ZQ_EXPORT void dllStart(ILibManager* pm)
{
    CREATE_LIB(pm, HttpLib)

};

ZQ_EXPORT void dllStop(ILibManager* pm)
{													  
    DESTROY_LIB(pm, HttpLib)
};								  

#endif


const int HttpLib::getLibVersion()
{
    return 0;
}

const std::string HttpLib::getLibName()
{
	return GET_CLASS_NAME(HttpLib);
}

void HttpLib::install()
{
	REGISTER_MODULE(libManager_, IHttpServerModule, HttpServerModule)
	REGISTER_MODULE(libManager_, IHttpClientModule, HttpClientModule)
}

void HttpLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IHttpClientModule, HttpClientModule)
	UNREGISTER_MODULE(libManager_, IHttpServerModule, HttpServerModule)
}