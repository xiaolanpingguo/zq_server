//#pragma once 
//
//#include <list>
//#include "IHttpServer.h"
//#include "multipart_reader.h"
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <windows.h>
//#include <io.h>
//#include <fcntl.h>
//#ifndef S_ISDIR
//#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
//#endif
//#ifndef LIBEVENT_SRC
//#pragma comment( lib, "libevent.lib")
//#endif
//
//#else
//
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <signal.h>
//#include <unistd.h>
//#include <dirent.h>
//#include <atomic>
//#include <stdio.h>
//#include <iostream>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//
//#endif
//
//#include <event2/bufferevent.h>
//#include "event2/bufferevent_struct.h"
//#include "event2/event.h"
//#include <event2/http.h>
//#include <event2/buffer.h>
//#include <event2/util.h>
//#include <event2/keyvalq_struct.h>
//
////#define ENABLE_SSL
//
//#ifdef ENABLE_SSL
//#include <openssl/ssl.h>
//#endif
//
//
//namespace zq {
//
//
//class HttpServer : public IHttpServer
//{
//public:
//	HttpServer();
//
//	template<typename BaseType>
//	HttpServer(BaseType* pBaseType, bool (BaseType::*handleRecieve)(const stHttpRequest& req))
//	{
//		base_ = NULL;
//		receiveCb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1);
//	}
//
//	virtual ~HttpServer();
//
//	virtual bool run();
//
//	virtual int initServer(const unsigned short nPort, const std::string& crt_file = "", const std::string& key_file = "");
//
//	virtual bool responseMsg(const stHttpRequest& req, const std::string& strMsg, EnWebStatus code, const std::string& strReason = "OK");
//
//private:
//
//	static struct ::bufferevent* bevcb(struct event_base *base, void *arg);
//	static void listener_cb(struct evhttp_request* req, void* arg);
//	static void log_cb(int severity, const char* data);
//
//	// post相关
//	stHttpRequest* allocateHttpRequest();
//	bool parsePost(const stHttpRequest& req);
//	EnContentTypeReq getContentType(const stHttpRequest& req);
//
//	bool handleStringBody(const stHttpRequest& req);
//	bool handleMultipart(const stHttpRequest& req);
//	bool handleCctetStream(const stHttpRequest& req);
//	bool handleFormUrlencoded(const stHttpRequest& req);
//	bool handleChunked(const stHttpRequest& req);
//
//	// 解析multi数据相关
//	void onPartBeginCb(const multipart_headers& headers);
//	void onPartDataCb(const char* data, size_t len);
//	void onPartEndCb();
//	void onPartDataEndCb();
//
//private:
//
//	int64 index_ = 0;
//
//	struct event_base* base_;
//	HTTP_RECEIVE_FUNCTOR receiveCb_;
//	HTTP_FILTER_FUNCTOR filter_;
//
//	MultipartReader multipartReader_;
//
//	std::map<int64, stHttpRequest*> httpRequestMap_;
//	std::list<stHttpRequest*> httpRequestPool_;
//
//#ifdef ENABLE_SSL
//	SSL_CTX* sslCtx_ = nullptr;
//#endif
//};
//
//}
