//#pragma once
//
//
//#include "interface_header/IHttpClientModule.h"
//#include "interface_header/ILogModule.h"
//#include "interface_header/IKernelModule.h"
//
//namespace zq {
//
//class HttpClientModule : public IHttpClientModule
//{
//
//public:
//	HttpClientModule(ILibManager* p);
//
//	virtual ~HttpClientModule();
//
//	virtual bool init();
//
//	virtual bool initEnd();
//
//	virtual bool run();
//
//	int syncPost(const std::string& strUri, const std::string& strData, std::string& strResData, int timeout = 5) override;
//	bool asyncPost(const std::string& strUrl, const std::string& strPostData, HTTP_RESP_FUNCTOR&& cb = nullptr) override;
//
//	int syncGet(const std::string& strUri, std::string& strResData, int timeout = 5) override;
//	bool asyncGet(const std::string& strUrl, HTTP_RESP_FUNCTOR&& cb = nullptr) override;
//
//
//protected:
//
//	struct RespData
//	{
//		bool complete = false;
//		int time = 0;
//		int state_code = 0;
//		std::string strRespData;
//	};
//
//	void callBack(const Guid id, const int state_code, const std::string& strRespData);
//
//private:
//	IKernelModule * kernelModule_;
//	IHttpClient* httpClient_;
//	std::map<std::string, std::string> defaultHttpHeaders_;
//	MapEx<Guid, RespData> respDataMap_;
//};
//
//}
