//#include "httpserver_module.h"
//
//using namespace zq;
//
//HttpServerModule::HttpServerModule(ILibManager* p)
//{
//    libManager_ = p;
//    m_pHttpServer = nullptr;
//}
//
//HttpServerModule::~HttpServerModule()
//{
//    if (m_pHttpServer)
//    {
//        delete m_pHttpServer;
//        m_pHttpServer = nullptr;
//    }
//}
//
//bool HttpServerModule::run()
//{
//    if (m_pHttpServer)
//    {
//        m_pHttpServer->run();
//    }
//
//    return true;
//}
//
//int HttpServerModule::initServer(const unsigned short nPort)
//{
//    m_pHttpServer = new HttpServer(this, &HttpServerModule::onReceiveNetPack);
//    return m_pHttpServer->initServer(nPort);
//}
//
//bool HttpServerModule::onReceiveNetPack(const stHttpRequest& req)
//{
//	auto it = msgCbMap_.find(req.type);
//	if (it != msgCbMap_.end())
//	{
//		auto itPath = it->second.find(req.path);
//		if (it->second.end() != itPath)
//		{
//			try
//			{
//				(itPath->second)(req);
//			}
//			catch (const std::exception& e)
//			{
//				responseMsg(req, e.what(), EnWebStatus::WEB_INTER_ERROR);
//			}
//			catch (...)
//			{
//				responseMsg(req, "unkown error", EnWebStatus::WEB_INTER_ERROR);
//			}
//
//			return true;
//		}
//	}
//
//	return responseMsg(req, "", EnWebStatus::WEB_ERROR);
//}
//
//EnWebStatus HttpServerModule::onFilterPack(const stHttpRequest& req)
//{
//	auto itPath = msgFliterMap_.find(req.path);
//	if (msgFliterMap_.end() != itPath)
//	{
//		return (itPath->second)(req);
//	}
//
//	return EnWebStatus::WEB_OK;
//}
//
//bool HttpServerModule::addMsgCB(const std::string& strPath, const EnHttpType eRequestType, HTTP_RECEIVE_FUNCTOR&& cb)
//{
//	msgCbMap_[eRequestType][strPath] = std::move(cb);
//	return false;
//}
//
//bool HttpServerModule::addFilterCB(const std::string& strPath, HTTP_FILTER_FUNCTOR&& cb)
//{
//	auto it = msgFliterMap_.find(strPath);
//	if (it == msgFliterMap_.end())
//	{
//		msgFliterMap_.insert(std::make_pair(strPath, std::move(cb)));
//	}
//
//    return true;
//}
//
//bool HttpServerModule::responseMsg(const stHttpRequest& req, const std::string& strMsg, EnWebStatus code, const std::string& strReason)
//{
//    return m_pHttpServer->responseMsg(req, strMsg, code, strReason);
//}
