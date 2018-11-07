//#pragma once 
//
//#include "baselib/network/HttpServer.h"
//#include "baselib/interface_header/IHttpServerModule.h"
//#include "baselib/interface_header/ILogModule.h"
//
//namespace zq {
//
//class HttpServerModule : public IHttpServerModule
//{
//public:
//	HttpServerModule(ILibManager* p);
//	virtual ~HttpServerModule();
//
//public:
//
//	virtual int initServer(const unsigned short nPort);
//
//	virtual bool run();
//
//	virtual bool responseMsg(const stHttpRequest& req, const std::string& strMsg, EnWebStatus code = EnWebStatus::WEB_OK, const std::string& reason = "OK");
//
//private:
//
//	virtual bool onReceiveNetPack(const stHttpRequest& req);
//	virtual EnWebStatus onFilterPack(const stHttpRequest& req);
//
//	virtual bool addMsgCB(const std::string& strPath, const EnHttpType eRequestType, HTTP_RECEIVE_FUNCTOR&& cb);
//	virtual bool addFilterCB(const std::string& strPath, HTTP_FILTER_FUNCTOR&& cb);
//
//private:
//	ILogModule * logModule_;
//	IHttpServer* m_pHttpServer;
//
//	std::map<EnHttpType, std::map<std::string, HTTP_RECEIVE_FUNCTOR>> msgCbMap_;
//	std::map<std::string, HTTP_FILTER_FUNCTOR> msgFliterMap_;
//};
//
//}
//
