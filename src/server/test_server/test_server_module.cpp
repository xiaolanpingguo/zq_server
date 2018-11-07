#include "test_server_module.h"

#include "baselib/message/config_define.hpp"
#include "baselib/message/game_ss.pb.h"

#include "baselib/core/object.h"

using namespace zq;
using namespace std;


bool TestServerModule::init()
{
	elementModule_ = libManager_->findModule<IElementModule>();
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	logModule_ = libManager_->findModule<ILogModule>();
	httpServerModule_ = libManager_->findModule<IHttpServerModule>();
	httpClientModule_ = libManager_->findModule<IHttpClientModule>();
	eventModule_ = libManager_->findModule<IEventModule>();
	messageDispatcherModule_ = libManager_->findModule<IMessageDispatcherModule>();

	return true;
}

bool TestServerModule::initEnd()
{
	{
		DataList xData;
		xData.AddInt(111);

		std::cout << "TestServerModule::initEnd" << std::endl;

		IObject* pObject = new CObject(Guid(0, 1), libManager_);

		pObject->getPropertyManager()->addProperty(pObject->self(), "string_type", TDATA_STRING);
		pObject->getPropertyManager()->addProperty(pObject->self(), "int_type", TDATA_INT);

		pObject->setPropertyInt("int_type", 1111);
		int n1 = pObject->getPropertyInt("int_type");

		std::cout << "int_type:" << n1 << std::endl;

		const std::string& default_value = pObject->getPropertyString("string_type");
		std::cout << "default_value:" << default_value << std::endl;

		pObject->setPropertyString("string_type", "string_type_fater");
		const std::string& after_change = pObject->getPropertyString("string_type");
		std::cout << "after_change:" << after_change << std::endl;


		pObject->addPropertyCallBack("int_type", [this](const Guid& self, const std::string& strProperty, const AbstractData& oldVar, const AbstractData& newVar)
		{  	std::cout << "OnPropertyCallBackEvent Property: " << strProperty << " OldValue: " << oldVar.getInt() << " NewValue: " << newVar.getInt() << std::endl;
		return 0;
		});

		pObject->setPropertyInt("int_type", 2222);

		int n2 = pObject->getPropertyInt("int_type");
		std::cout << "Property World:" << n2 << std::endl;


		kernelModule_->registerClassCallBack(config::Server::this_name(), [this](const Guid& self, const std::string& strClassName, const CLASS_OBJECT_EVENT event, const DataList& arg)
		{
			std::cout << "OnClassCallBackEvent ClassName: " << strClassName << " ID: " << self.nData64 << " Event: " << event << std::endl;
			if (event == COE_CREATE_HASDATA)
			{
				eventModule_->addEventCallBack(self, EnEventDefine(1), this, &TestServerModule::onEvent);
			}
			return 0; });
	}

	{
		std::shared_ptr<IObject> pObject = kernelModule_->createObject(Guid(0, 10), 1, 0, config::Server::this_name(), "", DataList());
		if (!pObject)
		{
			return false;
		}

		pObject->getPropertyManager()->addProperty(pObject->self(), "Hello", TDATA_STRING);
		pObject->getPropertyManager()->addProperty(pObject->self(), "World", TDATA_INT);

		pObject->setPropertyInt("World", 1111);
		pObject->setPropertyString("Hello", "2222");

		std::shared_ptr<IRecord> irecord = kernelModule_->findRecord(pObject->self(), config::Server::need_conn_type::this_name());
		if (irecord)
		{
			std::shared_ptr<DataList> xDataList = irecord->getInitData();
			xDataList->setInt(2, 100);
			xDataList->setInt(3, 200);
			xDataList->setInt(4, 300);
			irecord->addRow(-1, *xDataList);
		}

		auto recordmgr = pObject->getRecordManager();
		recordmgr->setRecordInt(config::Server::need_conn_type::this_name(), 0, "type1", 1000);
		recordmgr->setRecordInt(config::Server::need_conn_type::this_name(), 0, "type5", 1000);

		cout << recordmgr->getRecordInt(config::Server::need_conn_type::this_name(), 0, "type1");
		cout << recordmgr->getRecordInt(config::Server::need_conn_type::this_name(), 0, "type5");

		eventModule_->doEvent(pObject->self(), EnEventDefine(1), DataList() << int(100) << "200");
	}

	return true;
}

int TestServerModule::onEvent(const Guid& self, const EnEventDefine event, const DataList& arg)
{

	std::cout << "OnEvent EventID: " << event << " self: " << self.nData64 << " argList: " << arg.Int(0) << " " << " " << arg.String(1) << std::endl;

	kernelModule_->setPropertyInt(self, "Hello", arg.Int(0));
	kernelModule_->setPropertyString(self, "Hello", arg.String(1));

	return 0;
}

bool TestServerModule::shut()
{
	return true;
}

bool TestServerModule::run()
{
	test();
	test_http_client();

	return true;
}

void TestServerModule::processMessage(IChannel* channel, const char* data, size_t len, int msgid)
{
	// 50000次请求，花了4539ms，不到5秒
	using namespace std::chrono;

	static uint64 cost_time = 0;
	static uint64 begin_time = 0;
	static uint64 end_time = 0;
	static int count = 0;
	if (begin_time == 0)
	{
		begin_time = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
	}

	count++;

	//if (count % 10000 == 0)
	{
		end_time = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
		cost_time = end_time - begin_time;

		std::cout << "begin_time: " << begin_time << std::endl;
		std::cout << "end_time: " << end_time << std::endl;
		std::cout << "cost_time: " << cost_time << "ms" << std::endl;
		std::cout << "count: " << count << std::endl;
			
		logModule_->info(fmt::format("cost_time: {}", cost_time));
		logModule_->warning(fmt::format("cost_time: {}", cost_time));
		logModule_->error(fmt::format("cost_time: {}", cost_time));
		LOG_INFO << "begin_time: " << begin_time;
		LOG_WARN << "end_time: " << end_time;
		LOG_ERROR << "count: " << count;
	}
}


void TestServerModule::test()
{

}

void TestServerModule::test_http_client()
{
}

