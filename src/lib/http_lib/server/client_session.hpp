//#pragma once
//
//
//#include <fstream>
//#include "picohttpparser.h"
//#include "multipart_reader.hpp"
//
//namespace zq{
//
//	
//class session : public std::enable_shared_from_this<session>
//{
//public:
//	// Resolver and socket require an io_context
//	explicit session(boost::asio::io_context& ioc)
//		: resolver_(ioc)
//		, socket_(ioc)
//	{
//	}
//
//	// Start the asynchronous operation
//	void run(char const* host, char const* port, char const* target, int version)
//	{
//		// Set up an HTTP GET request message
//		req_.version(version);
//		req_.method(http::verb::get);
//		req_.target(target);
//		req_.set(http::field::host, host);
//		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//
//		// Look up the domain name
//		resolver_.async_resolve(host, port,
//			std::bind(&session::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
//	}
//
//	void on_resolve(boost::system::error_code ec, tcp::resolver::results_type results)
//	{
//		if (ec)
//			return fail(ec, "resolve");
//
//		// Make the connection on the IP address we get from a lookup
//		boost::asio::async_connect(socket_, results.begin(), results.end(),
//			std::bind(&session::on_connect, shared_from_this(), std::placeholders::_1));
//	}
//
//	void on_connect(boost::system::error_code ec)
//	{
//		if (ec)
//			return fail(ec, "connect");
//
//		// Send the HTTP request to the remote host
//		http::async_write(socket_, req_,
//			std::bind(&session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
//	}
//
//	void on_write(boost::system::error_code ec, std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		if (ec)
//			return fail(ec, "write");
//
//		// Receive the HTTP response
//		http::async_read(socket_, buffer_, res_,
//			std::bind(&session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
//	}
//
//	void on_read(boost::system::error_code ec, std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		if (ec)
//			return fail(ec, "read");
//
//		// Write the message to standard out
//		std::cout << res_ << std::endl;
//
//		// Gracefully close the socket
//		socket_.shutdown(tcp::socket::shutdown_both, ec);
//
//		// not_connected happens sometimes so don't bother reporting it.
//		if (ec && ec != boost::system::errc::not_connected)
//			return fail(ec, "shutdown");
//
//		// If we get here then the connection is closed gracefully
//	}
//
//private:
//	tcp::resolver resolver_;
//	tcp::socket socket_;
//	boost::beast::flat_buffer buffer_; // (Must persist between reads)
//	http::request<http::empty_body> req_;
//	http::response<http::string_body> res_;
//};
//
//
//}