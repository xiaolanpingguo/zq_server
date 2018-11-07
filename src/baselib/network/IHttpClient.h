#pragma once

#include <functional>
#include <memory>
#include <map>
#include "interface_header/uuid.h"


namespace zq {

using HTTP_RESP_FUNCTOR = std::function<void(const Guid id, const int state_code, const std::string& strRespData)>;
class IHttpClient
{
public:
	virtual ~IHttpClient()
	{}

	virtual bool run() = 0;

	virtual bool init() = 0;

	virtual bool doGet(const std::string& strUri, HTTP_RESP_FUNCTOR&& cb,
		const std::map<std::string, std::string>& headers, const Guid id = Guid()) = 0;

	virtual bool doPost(const std::string& strUri, const std::string& strPostData, HTTP_RESP_FUNCTOR&& cb,
		const std::map<std::string, std::string>& headers, const Guid id = Guid()) = 0;


};

}

