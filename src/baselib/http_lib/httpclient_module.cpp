#include "httpclient_module.h"
#include "client/async_session.hpp"
#include "client/async_session_ssl.hpp"
#include "client/coro_session.hpp"
#include "client/coro_session_ssl.hpp"
#include "client/sync_session.hpp"
#include "client/sync_session_ssl.hpp"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libboost_system-vc141-mt-gd-x64-1_67.lib")
#pragma comment(lib, "libboost_date_time-vc141-mt-gd-x64-1_67.lib")
#pragma comment(lib, "libboost_context-vc141-mt-gd-x64-1_67.lib")
#pragma comment(lib, "libboost_coroutine-vc141-mt-gd-x64-1_67.lib")
#endif

using namespace zq;



static inline http::verb getMethod(C_HTTP_METHOD method)
{
	switch (method)
	{
	case C_HTTP_METHOD::GET: return	http::verb::get;
	case C_HTTP_METHOD::POST: return http::verb::post;
	}

	return http::verb::unknown;
}

HttpClientModule::HttpClientModule(ILibManager* p)
	:ctx_(ssl::context::sslv23_client)
{
    libManager_ = p;
}

HttpClientModule::~HttpClientModule()
{

}

bool HttpClientModule::init()
{

    return true;
}

bool HttpClientModule::initEnd()
{
	std::string cert = "";
	boost::system::error_code ec;
	if (!cert.empty())
	{
		ctx_.add_certificate_authority(boost::asio::buffer(cert.data(), cert.size()), ec);
		if (ec)
			return false;

		ctx_.set_verify_mode(ssl::verify_peer);
	}

	return true;
}

bool HttpClientModule::run()
{
	ioc_.poll();
    return true;
}

bool HttpClientModule::shut()
{
	ioc_.stop();
	return true;
}

bool HttpClientModule::syncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	std::string& res, int timeout)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	std::make_shared<SyncSession>(ioc_)->run(verb_method, host, port, path, res);
	auto req_time = libManager_->getNowTime();
	while (res.empty())
	{
		auto now = libManager_->getNowTime();
		if (req_time + timeout < now)
		{
			break;
		}

		ioc_.poll();
	}

	return true;
}

bool HttpClientModule::syncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	std::string& res, int timeout)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	std::make_shared<SyncSessionSSL>(ioc_, ctx_)->run(verb_method, host, port, path, res);
	auto req_time = libManager_->getNowTime();
	while (res.empty())
	{
		auto now = libManager_->getNowTime();
		if (req_time + timeout < now)
		{
			break;
		}

		ioc_.poll();
	}

	return true;
}

bool HttpClientModule::asyncRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	C_HTTP_RESP_FUNCTOR&& cb)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	std::make_shared<AsyncSession>(ioc_, std::move(cb))->run(verb_method, host, port, path);

	return true;
}

bool HttpClientModule::asyncRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	C_HTTP_RESP_FUNCTOR&& cb)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	std::make_shared<AsyncSessionSSL>(ioc_, ctx_, std::move(cb))->run(verb_method, host, port, path);

	return true;
}

bool HttpClientModule::coroRequst(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	std::string& res)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	boost::asio::spawn(ioc_,
		std::bind(&CoroSession::do_session, verb_method, host, port, path, res, 11, std::ref(ioc_), std::placeholders::_1));

	return true;
}

bool HttpClientModule::coroRequstSSL(C_HTTP_METHOD method, const std::string& host, const std::string& port, const std::string& path,
	std::string& res)
{
	http::verb verb_method = getMethod(method);
	if (verb_method == http::verb::unknown)
	{
		return false;
	}

	boost::asio::spawn(ioc_,
		std::bind(&CoroSessionSSL::do_session, verb_method, host, port, path, res,
			11, std::ref(ioc_), std::ref(ctx_), std::placeholders::_1));

	return true;
}
