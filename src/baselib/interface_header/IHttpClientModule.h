#pragma once

#include "baselib/interface_header/IModule.h"

namespace zq {

enum class C_HTTP_METHOD
{
	GET,
	POST,
};

using C_HTTP_RESP_FUNCTOR = std::function<void(const std::string&)>;
class IHttpClientModule : public IModule
{
public:
	virtual ~IHttpClientModule() {};

	virtual bool syncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path, 
		std::string& res, int timeout = 5) = 0;
	virtual bool syncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		std::string& res, int timeout = 5) = 0;

	virtual bool asyncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		C_HTTP_RESP_FUNCTOR&& cb = nullptr) = 0;
	virtual bool asyncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
		C_HTTP_RESP_FUNCTOR&& cb = nullptr) = 0;

	virtual bool coroRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path, std::string& res) = 0;
	virtual bool coroRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path, std::string& res) = 0;
};

}
