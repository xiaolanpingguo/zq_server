//#include <iostream>
//#include "server/server.hpp"
//#include "client/async_session.hpp"
//#include "client/sync_session.hpp"
//#include "client/coro_session.hpp"
//using namespace zq;
//using namespace std;
//
//#pragma comment(lib, "libboost_system-vc141-mt-gd-x64-1_67.lib")
//#pragma comment(lib, "libboost_date_time-vc141-mt-gd-x64-1_67.lib")	
//#pragma comment(lib, "libboost_regex-vc141-mt-gd-x64-1_67.lib")
//
//boost::asio::io_service ioc_;
//void f()
//{
//	//std::this_thread::sleep_for(std::chrono::seconds(1));
//	cout << "111" << endl;
//}
//
//int main() 
//{
////	zqlog::LogSingle::getInstance().init(zqlog::GuaranteedLogger(), "log/", "http_server");
////
////	const int max_thread_num = 4;
////	http_server_single_thread server;
////
////#ifdef ZQ_ENABLE_SSL
////	server.init_ssl_context(true, [](auto, auto) { return "123456"; }, "server.crt", "server.key", "dh1024.pem");
////	bool r = server.listen("0.0.0.0", "https");
////#else
////	bool r = server.listen("0.0.0.0", "8080");
////#endif
////	if (!r) 
////	{
////		LOG_INFO << "listen failed";
////		return -1;
////	}
////
////	server.set_http_handler(GET, "/get", [](request& req, response& res) 
////	{
////		const auto& querys = req.queries();
////		cout << "raw querys" << endl;
////		for (const auto& ref : querys)
////		{
////			cout << ref.first << ":" << ref.second << endl;
////		}
////
////		cout << "after encode" << endl;
////		for (const auto& ref : querys)
////		{
////			cout << ref.first << ":" << req.get_query_value(ref.first) << endl;
////		}
////
////		res.set_status_and_content(status_type::ok,"get ok");
////	});
////
////	server.set_http_handler(POST, "/json", [](request& req, response& res)
////	{
////		cout << "body:" << req.body() << endl;
////
////		res.set_status_and_content(status_type::ok, "post /json ok");
////	});
////
////	std::thread thr(std::bind(&http_server_single_thread::run, &server));
//
//
//// The io_context is required for all I/O
//	boost::asio::io_context ioc_;
//
//	// Launch the asynchronous operation
//	//boost::asio::spawn(ioc_, 
//	//	std::bind(&CoroSession::do_session, "127.0.0.1", "8080", "/", 11, std::ref(ioc_), std::placeholders::_1));
//
//
//	// Run the I/O service. The call will return when
//	// the get operation is complete.
//	ioc_.run();
//
//	ioc_.poll();
//
//	//thr.join();
//	cout << "exit..." << endl;
//	std::getchar();
//	return 0;
//}