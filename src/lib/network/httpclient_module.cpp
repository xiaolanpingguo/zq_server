//#include "httpclient_module.h"
//#include "httpclient.h"
//
//using namespace zq;
//
//#define DEFAULT_USER_AGENT "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Mobile Safari/537.36"
//
//HttpClientModule::HttpClientModule(ILibManager* p)
//{
//    libManager_ = p;
//    httpClient_ = new HttpClient();
//    defaultHttpHeaders_["Connection"] = "close";
//	defaultHttpHeaders_["Content-Type"] = "text/plain;text/html;application/x-www-form-urlencoded;charset=utf-8";
//	//defaultHttpHeaders_["Content-Type"] = "text/plain;text/html;application/x-www-form-urlencoded;charset=utf-8";
//    defaultHttpHeaders_["User-Agent"] = DEFAULT_USER_AGENT;
//    defaultHttpHeaders_["Cache-Control"] = "no-cache";
//}
//
//HttpClientModule::~HttpClientModule()
//{
//    delete httpClient_;
//    httpClient_ = NULL;
//}
//
//bool HttpClientModule::init()
//{
//    httpClient_->init();
//
//    return true;
//}
//
//bool HttpClientModule::initEnd()
//{
//	kernelModule_ = libManager_->findModule<IKernelModule>();
//	return true;
//}
//
//bool HttpClientModule::run()
//{
//    httpClient_->run();
//    return true;
//}
//
//int HttpClientModule::syncPost(const std::string& strUrl, const std::string& strData, std::string& strResData, int timeout)
//{
//	auto cb = std::bind(&HttpClientModule::callBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
//
//	Guid id = kernelModule_->createGUID();
//	httpClient_->doPost(strUrl, strData, std::move(cb), defaultHttpHeaders_, id);
//
//	time_t post_time = time(nullptr);
//
//	respDataMap_.addElement(id, std::shared_ptr<RespData>(new RespData()));
//	std::shared_ptr<RespData> resp_data = respDataMap_.getElement(id);
//	while (!resp_data->complete)
//	{
//		// 超时了
//		time_t now = time(nullptr);
//		if (post_time + timeout < now)
//		{
//			resp_data->state_code = -1;
//			break;
//		}
//
//		httpClient_->run();
//	}
//
//	strResData = resp_data->strRespData;
//	int error_code = resp_data->state_code;
//
//	respDataMap_.removeElement(id);
//
//	return error_code;
//}
//
//bool HttpClientModule::asyncPost(const std::string& strUrl, const std::string& strData, HTTP_RESP_FUNCTOR&& cb)
//{
//	return httpClient_->doPost(strUrl, strData, std::move(cb), defaultHttpHeaders_);
//}
//
//int HttpClientModule::syncGet(const std::string& strUri, std::string& strResData, int timeout)
//{
//	auto cb = std::bind(&HttpClientModule::callBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
//
//	Guid id = kernelModule_->createGUID();
//	httpClient_->doGet(strUri, std::move(cb), defaultHttpHeaders_ , id);
//
//	time_t get_time = time(nullptr);
//
//	respDataMap_.addElement(id, std::shared_ptr<RespData>(new RespData()));
//	std::shared_ptr<RespData> resp_data = respDataMap_.getElement(id);
//	while (!resp_data->complete)
//	{
//		// 超时了
//		time_t now = time(nullptr);
//		if (get_time + timeout < now)
//		{
//			resp_data->state_code = -1;
//			break;
//		}
//
//		httpClient_->run();
//	}
//
//	strResData = resp_data->strRespData;
//	int error_code = resp_data->state_code;
//
//	respDataMap_.removeElement(id);
//
//	return error_code;
//}
//
//bool HttpClientModule::asyncGet(const std::string& strUrl, HTTP_RESP_FUNCTOR&& cb)
//{
//	return httpClient_->doGet(strUrl, std::move(cb), defaultHttpHeaders_);
//}
//
//void HttpClientModule::callBack(const Guid id, const int state_code, const std::string & strRespData)
//{
//	std::shared_ptr<RespData> xRespData = respDataMap_.getElement(id);
//	if (xRespData)
//	{
//		xRespData->complete = true;
//		xRespData->state_code = state_code;
//		xRespData->strRespData = strRespData;
//	}
//}