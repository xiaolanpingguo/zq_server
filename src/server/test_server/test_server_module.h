#pragma once

#include "baselib/interface_header/IKernelModule.h"
#include "baselib/interface_header/IClassModule.h"
#include "baselib/interface_header/ILogModule.h"
#include "baselib/interface_header/IElementModule.h"
#include "baselib/interface_header/ITestServerModule.h"
#include "baselib/interface_header/IEventModule.h"
#include "baselib/interface_header/IHttpServerModule.h"
#include "baselib/interface_header/IHttpClientModule.h"
#include "baselib/interface_header/IMessageDispatcherModule.h"


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

	int onPropertyCallBackEvent(const Guid& self, const std::string& strProperty, const AbstractData& oldVar, const AbstractData& newVar);

	int onClassCallBackEvent(const Guid& self, const std::string& strClassName, const CLASS_OBJECT_EVENT event, const DataList& arg);
	int onEvent(const Guid& self, const EnEventDefine event, const DataList& arg);

	void processMessage(IChannel* channel, const char* data, size_t len, int msgid);

private:

	void test();
	void test_http_client();

private:

	IElementModule * elementModule_;
	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	ILogModule* logModule_;
	IEventModule* eventModule_;
	IHttpServerModule* httpServerModule_;
	IHttpClientModule* httpClientModule_;
	IMessageDispatcherModule* messageDispatcherModule_;
};

}

