#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <memory>
#include <string>

namespace zq{


using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


class AsyncSession : public std::enable_shared_from_this<AsyncSession>
{
public:
	explicit AsyncSession(boost::asio::io_context& ioc, std::function<void(const std::string&)>&& cb)
		: resolver_(ioc), socket_(ioc), cb_(std::move(cb))
	{
	}

	void run(http::verb method, const std::string& host, const std::string& port, const std::string& path, int version = 11)
	{
		req_.version(version);
		req_.method(method);
		req_.target(path);
		req_.set(http::field::host, host);
		req_.set(http::field::user_agent, "zq");

		// Look up the domain name
		resolver_.async_resolve(host, port,
			std::bind(&AsyncSession::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void on_resolve(const boost::system::error_code& ec, tcp::resolver::results_type results)
	{
		if (ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(socket_, results.begin(), results.end(),
			std::bind(&AsyncSession::on_connect, shared_from_this(), std::placeholders::_1));
	}

	void on_connect(const boost::system::error_code& ec)
	{
		if (ec)
			return fail(ec, "connect");

		// Send the HTTP request to the remote host
		http::async_write(socket_, req_,
			std::bind(&AsyncSession::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void on_write(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "write");

		// Receive the HTTP response
		http::async_read(socket_, buffer_, res_,
			std::bind(&AsyncSession::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void on_read(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "read");

		// Write the message to standard out
		if (cb_)
		{
			cb_(res_.body());
		}

		// Gracefully close the socket
		boost::system::error_code err;
		socket_.shutdown(tcp::socket::shutdown_both, err);

		// If we get here then the connection is closed gracefully
	}

private:

	void fail(const boost::system::error_code& ec, char const* what)
	{
		if (cb_)
		{
			cb_("");
		}
	}

private:

	tcp::resolver resolver_;
	tcp::socket socket_;
	boost::beast::flat_buffer buffer_; // (Must persist between reads)
	http::request<http::empty_body> req_;
	http::response<http::string_body> res_;
	std::function<void(const std::string&)> cb_;
};


}
