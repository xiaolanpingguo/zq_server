#pragma once


#include "baselib/interface_header/IHttpClientModule.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ssl/context.hpp>

namespace zq {

class HttpClientModule : public IHttpClientModule
{

public:
	HttpClientModule(ILibManager* p);

	virtual ~HttpClientModule();

	virtual bool init();
	virtual bool initEnd();
	virtual bool shut();
	virtual bool run();

	bool syncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		std::string& res, int timeout = 5) override;
	bool syncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		std::string& res, int timeout = 5) override;

	bool asyncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		C_HTTP_RESP_FUNCTOR&& cb = nullptr) override;
	bool asyncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		C_HTTP_RESP_FUNCTOR&& cb = nullptr) override;

	bool coroRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		std::string& res) override;
	bool coroRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		std::string& res) override;

private:

	boost::asio::io_service ioc_;
	boost::asio::ssl::context ctx_;
};

}
