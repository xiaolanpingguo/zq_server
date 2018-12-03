#pragma once

#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/ITestServerModule.h"
#include "interface_header/base/IEventModule.h"
#include "interface_header/base/IHttpServerModule.h"
#include "interface_header/base/IHttpClientModule.h"
#include "interface_header/base/IMessageDispatcherModule.h"
#include "interface_header/base/IRedislModule.h"


namespace zq {


class TestServerModule : public ITestServerModule
{

public:
	TestServerModule(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual bool init();
	bool initEnd() override;
	virtual bool shut();
	virtual bool run();

protected:

	int onPropertyCallBackEvent(const uuid& self, const std::string& strProperty, const VariantData& oldVar, const VariantData& newVar);

	int onClassCallBackEvent(const uuid& self, const std::string& strClassName, const CLASS_OBJECT_EVENT event);

	void processMessage(IChannel* channel, const char* data, size_t len, int msgid);

private:

	void test();
	void test_http_client();

private:

	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	IHttpServerModule* httpServerModule_;
	IHttpClientModule* httpClientModule_;
	IMessageDispatcherModule* messageDispatcherModule_;
	IRedisModule* redisModule_;;
};

}

