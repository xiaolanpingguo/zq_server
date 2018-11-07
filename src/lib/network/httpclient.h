//#pragma once
//
//#include "IHttpClient.h"
//#include "IHttpServer.h"
//
//#include <event2/bufferevent.h>
//#include <event2/buffer.h>
//#include <event2/listener.h>
//#include <event2/util.h>
//#include <event2/http.h>
//#include <event2/thread.h>
//#include <event2/event_compat.h>
//#include <event2/bufferevent_struct.h>
//#include <event2/http_struct.h>
//#include <event2/event.h>
//
////#define ENABLE_SSL
//
//#ifdef ENABLE_SSL
//#include <openssl/ssl.h>
//#endif
//
//namespace zq {
//
//class HttpObject
//{
//public:
//	HttpObject(IHttpClient* pNet, struct bufferevent* pBev, HTTP_RESP_FUNCTOR&& cb, Guid id)
//	{
//		bev_ = pBev;
//		httpClient_ = pNet;
//		cb_ = std::move(cb);
//		guid_ = id;
//	}
//
//	virtual ~HttpObject()
//	{
//	}
//
//	Guid guid_;
//	bufferevent* bev_;
//	IHttpClient* httpClient_;
//	HTTP_RESP_FUNCTOR cb_;
//};
//
//
//class HttpClient : public IHttpClient
//{
//public:
//
//	HttpClient(int nRetry = 2, int nTimeoutSec = 2);
//	virtual ~HttpClient();
//
//public:
//	virtual bool run();
//
//	virtual bool init();
//
//	virtual bool doGet(const std::string& strUri, HTTP_RESP_FUNCTOR&& cb,
//		const std::map<std::string, std::string>& headers, const Guid id = Guid());
//
//	virtual bool doPost(const std::string& strUri, const std::string& strPostData, HTTP_RESP_FUNCTOR&& cb,
//		const std::map<std::string, std::string>& headers, const Guid id = Guid());
//
//private:
//	static void onHttpReqDone(struct evhttp_request* req, void* ctx);
//
//	bool makeRequest(const std::string& strUri,
//		HTTP_RESP_FUNCTOR&& cb,
//		const std::string& strPostData,
//		const std::map<std::string, std::string>& headers,
//		const EnHttpType eHttpType,
//		const Guid id = Guid());
//
//private:
//	std::string userAgent_;
//	struct event_base* eventBase_ = nullptr;
//
//	int retry_ = 2;
//	int timeOut_ = 2;
//
//	std::vector<HttpObject*> listHttpObjects_;
//
//#ifdef ENABLE_SSL
//	SSL_CTX* sslCtx_ = nullptr;
//#endif
//};
//
//
//}
//
