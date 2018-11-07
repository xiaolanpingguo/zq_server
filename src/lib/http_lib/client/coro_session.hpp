#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <memory>
#include <string>

namespace zq{

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class CoroSession : public std::enable_shared_from_this<CoroSession>
{
public:
	static void fail(const boost::system::error_code& ec, const std::string& what)
	{

	}

	static void do_session(http::verb method, std::string const& host, std::string const& port, std::string const& path, 
		std::string& reponce, int version, boost::asio::io_context& ioc, boost::asio::yield_context yield)
	{
		boost::system::error_code ec;

		// These objects perform our I/O
		tcp::resolver resolver{ ioc };
		tcp::socket socket{ ioc };

		// Look up the domain name
		auto const results = resolver.async_resolve(host, port, yield[ec]);
		if (ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(socket, results.begin(), results.end(), yield[ec]);
		if (ec)
			return fail(ec, "connect");

		// Set up an HTTP GET request message
		http::request<http::string_body> req{ method, path, version };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, "zq");

		// Send the HTTP request to the remote host
		http::async_write(socket, req, yield[ec]);
		if (ec)
			return fail(ec, "write");

		// This buffer is used for reading and must be persisted
		boost::beast::flat_buffer b;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::async_read(socket, b, res, yield[ec]);
		if (ec)
			return fail(ec, "read");

		// Write the message to standard out
		reponce = res.body();

		// Gracefully close the socket
		socket.shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != boost::system::errc::not_connected)
			return fail(ec, "shutdown");

		// If we get here then the connection is closed gracefully
	}
};


}
