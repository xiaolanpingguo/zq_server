#pragma once


#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>


namespace zq{

using tcp = boost::asio::ip::tcp;    
namespace http = boost::beast::http;    

class SyncSession : public std::enable_shared_from_this<SyncSession>
{
public:
	explicit SyncSession(boost::asio::io_context& ioc)
		: resolver_(ioc)
		, socket_(ioc)
	{
	}

	void run(http::verb method, const std::string& host, const std::string& port, const std::string& path, std::string& responce, int version = 11)
	{
		responce.clear();

		try
		{
			auto const results = resolver_.resolve(host, port);
			boost::asio::connect(socket_, results.begin(), results.end());

			// Set up an HTTP GET request message
			http::request<http::string_body> req{ method, path, version };
			req.set(http::field::host, host);
			req.set(http::field::user_agent, "zq");

			// Send the HTTP request to the remote host
			http::write(socket_, req);

			// This buffer is used for reading and must be persisted
			boost::beast::flat_buffer buffer;

			// Declare a container to hold the response
			http::response<http::string_body> res;

			// Receive the HTTP response
			http::read(socket_, buffer, res);

			// Write the message to standard out
			responce = res.body();

			// Gracefully close the socket
			boost::system::error_code ec;
			socket_.shutdown(tcp::socket::shutdown_both, ec);

			// not_connected happens sometimes
			// so don't bother reporting it.
			if (ec && ec != boost::system::errc::not_connected)
				throw boost::system::system_error{ ec };
		}
		catch (const std::exception& /*e*/)
		{

		}
	}

private:

	tcp::resolver resolver_;
	tcp::socket socket_;
};


}
