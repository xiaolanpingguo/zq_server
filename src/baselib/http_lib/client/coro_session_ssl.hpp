#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <functional>
#include <memory>
#include <string>

namespace zq{

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class CoroSessionSSL
{
public:

	static void fail(boost::system::error_code ec, const std::string& what)
	{
		
	}

	static void do_session(http::verb method,std::string const& host, std::string const& port, std::string const& target, 
		std::string& reponce, int version, boost::asio::io_context& ioc, ssl::context& ctx, boost::asio::yield_context yield)
	{
		boost::system::error_code ec;

		// These objects perform our I/O
		tcp::resolver resolver{ ioc };
		ssl::stream<tcp::socket> stream{ ioc, ctx };

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
		{
			ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());			
			return fail(ec, ec.message());
		}

		// Look up the domain name
		auto const results = resolver.async_resolve(host, port, yield[ec]);
		if (ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(stream.next_layer(), results.begin(), results.end(), yield[ec]);
		if (ec)
			return fail(ec, "connect");

		// Perform the SSL handshake
		stream.async_handshake(ssl::stream_base::client, yield[ec]);
		if (ec)
			return fail(ec, "handshake");

		// Set up an HTTP GET request message
		http::request<http::string_body> req{ method, target, version };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, "zq");

		// Send the HTTP request to the remote host
		http::async_write(stream, req, yield[ec]);
		if (ec)
			return fail(ec, "write");

		// This buffer is used for reading and must be persisted
		boost::beast::flat_buffer b;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::async_read(stream, b, res, yield[ec]);
		if (ec)
			return fail(ec, "read");

		// Write the message to standard out
		reponce = res.body();

		// Gracefully close the stream
		stream.async_shutdown(yield[ec]);
		if (ec == boost::asio::error::eof)
		{
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec.assign(0, ec.category());
		}
		if (ec)
			return fail(ec, "shutdown");

		// If we get here then the connection is closed gracefully
	}
};

}
