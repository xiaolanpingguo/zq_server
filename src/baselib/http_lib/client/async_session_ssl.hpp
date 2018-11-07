#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
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


class AsyncSessionSSL : public std::enable_shared_from_this<AsyncSessionSSL>
{
public:
    explicit AsyncSessionSSL(boost::asio::io_context& ioc, ssl::context& ctx, std::function<void(const std::string&)>&& cb)
        : resolver_(ioc), stream_(ioc, ctx), cb_(std::move(cb))
    {
    }

    void run(http::verb method, const std::string& host, const std::string& port, const std::string& path, int version = 11)
    {
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(!SSL_set_tlsext_host_name(stream_.native_handle(), host.c_str()))
        {
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
			return fail(ec, ec.message());
        }

        // Set up an HTTP GET request message
        req_.version(version);
        req_.method(http::verb::get);
        req_.target(path);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, "zq");

        // Look up the domain name
        resolver_.async_resolve(host, port,
            std::bind(&AsyncSessionSSL::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_resolve(boost::system::error_code ec, tcp::resolver::results_type results)
    {
        if(ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(stream_.next_layer(), results.begin(), results.end(),
            std::bind(&AsyncSessionSSL::on_connect, shared_from_this(), std::placeholders::_1));
    }

    void on_connect(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        stream_.async_handshake(ssl::stream_base::client,
			std::bind(&AsyncSessionSSL::on_handshake, shared_from_this(), std::placeholders::_1));
    }

    void on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            std::bind(&AsyncSessionSSL::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_write(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");
        
        // Receive the HTTP response
        http::async_read(stream_, buffer_, res_,
            std::bind(&AsyncSessionSSL::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_read(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // Write the message to standard out
		if (cb_)
		{
			cb_(res_.body());
		}

        // Gracefully close the stream
        stream_.async_shutdown(
            std::bind(&AsyncSessionSSL::on_shutdown, shared_from_this(), std::placeholders::_1));
    }

    void on_shutdown(boost::system::error_code ec)
    {
        if(ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec.assign(0, ec.category());
        }
        if(ec)
            return fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }

private:

	void fail(const boost::system::error_code& ec, const std::string&)
	{
		if (cb_)
		{
			cb_("");
		}
	}

private:

	tcp::resolver resolver_;
	ssl::stream<tcp::socket> stream_;
	boost::beast::flat_buffer buffer_; // (Must persist between reads)
	http::request<http::empty_body> req_;
	http::response<http::string_body> res_;
	std::function<void(const std::string&)> cb_;
};


}