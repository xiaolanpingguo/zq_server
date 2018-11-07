#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <string>

namespace zq{

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


class SyncSessionSSL : public std::enable_shared_from_this<SyncSessionSSL>
{
public:
	// Resolver and socket require an io_context
	explicit SyncSessionSSL(boost::asio::io_context& ioc, ssl::context& ctx)
		: resolver_(ioc)
		, stream_(ioc, ctx)
	{
	}

	void run(http::verb method, const std::string& host, const std::string& port, const std::string& path, std::string& responce, int version = 11)
	{
		try
		{
			// Set SNI Hostname (many hosts need this to handshake successfully)
			if (!SSL_set_tlsext_host_name(stream_.native_handle(), host.c_str()))
			{
				boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
				throw boost::system::system_error{ ec };
			}

			// Look up the domain name
			auto const results = resolver_.resolve(host, port);

			// Make the connection on the IP address we get from a lookup
			boost::asio::connect(stream_.next_layer(), results.begin(), results.end());

			// Perform the SSL handshake
			stream_.handshake(ssl::stream_base::client);

			// Set up an HTTP GET request message
			http::request<http::string_body> req{ method, path, version };
			req.set(http::field::host, host);
			req.set(http::field::user_agent, "zq");

			// Send the HTTP request to the remote host
			http::write(stream_, req);

			// This buffer is used for reading and must be persisted
			boost::beast::flat_buffer buffer;

			// Declare a container to hold the response
			http::response<http::string_body> res;

			// Receive the HTTP response
			http::read(stream_, buffer, res);

			// Write the message to standard out
			responce = res.body();

			// Gracefully close the stream
			boost::system::error_code ec;
			stream_.shutdown(ec);
			if (ec == boost::asio::error::eof)
			{
				// Rationale:
				// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
				ec.assign(0, ec.category());
			}
			if (ec)
				throw boost::system::system_error{ ec };

			// If we get here then the connection is closed gracefully
		}
		catch (const std::exception& /*e*/)
		{

		}
	}

private:

	tcp::resolver resolver_;
	ssl::stream<tcp::socket> stream_;
};

}
