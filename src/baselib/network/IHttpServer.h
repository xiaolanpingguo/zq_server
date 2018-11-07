#pragma once 


#include <map>
#include <functional>
#include <memory>
#include "interface_header/platform.h"


namespace zq {


enum EnWebStatus
{
	WEB_OK = 200,
	WEB_AUTH = 401,
	WEB_ERROR = 404,
	WEB_INTER_ERROR = 500,
	WEB_TIMEOUT = 503,
};


enum EnHttpType
{
	EN_HTTP_REQ_GET = 1 << 0,
	EN_HTTP_REQ_POST = 1 << 1,
	EN_HTTP_REQ_HEAD = 1 << 2,
	EN_HTTP_REQ_PUT = 1 << 3,
	EN_HTTP_REQ_DELETE = 1 << 4,
	EN_HTTP_REQ_OPTIONS = 1 << 5,
	EN_HTTP_REQ_TRACE = 1 << 6,
	EN_HTTP_REQ_CONNECT = 1 << 7,
	EN_HTTP_REQ_PATCH = 1 << 8
};

enum class EnContentTypeReq
{
	EN_STRING,
	EN_MULTIPART,
	EN_URLENCODE,
	EN_CHUNKED,
	EN_OCTET_STREAM,
	EN_WEBSOCKET,
	EN_UNKNOWN,
};

enum class EnContentTypeRes
{
	EN_HTML,
	EN_JSON,
	EN_STRING,
	EN_NONE,
};

struct stHttpRequest
{
	stHttpRequest()
	{
		reset();
	}

	void reset()
	{
		id = 0;
		url.clear();
		path.clear();
		remoteHost.clear();
		//type
		body.clear();
		params.clear();
		headers.clear();
	}


	int64 id;
	void* req = nullptr;
	std::string url;
	std::string path;
	std::string remoteHost;
	EnHttpType type;
	std::string body;//when using post
	std::map<std::string, std::string> params;//when using get
	std::map<std::string, std::string> headers;
};

struct stPostRequest
{
	EnContentTypeReq content_type = EnContentTypeReq::EN_UNKNOWN;
	std::string boundary;
};


using HTTP_RECEIVE_FUNCTOR = std::function<bool(const stHttpRequest& req)>;
using HTTP_FILTER_FUNCTOR = std::function<EnWebStatus(const stHttpRequest& req)>;

class IHttpServer
{
public:

	virtual ~IHttpServer(){}
	virtual bool run() = 0;

	virtual int initServer(const unsigned short nPort, const std::string& crt_file = "", const std::string& key_file = "") = 0;

	virtual bool responseMsg(const stHttpRequest& req, const std::string& strMsg, EnWebStatus code, const std::string& strReason = "OK") = 0;
};

}