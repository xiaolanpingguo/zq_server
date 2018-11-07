#pragma once

#if defined(ASIO_STANDALONE)
//MSVC : define environment path 'ASIO_STANDALONE_INCLUDE', e.g. 'E:\bdlibs\asio-1.10.6\include'

#include <asio.hpp>
#ifdef ZQ_ENABLE_SSL
#include <asio/ssl.hpp>
#endif
#include <asio/steady_timer.hpp>
namespace boost
{
	namespace asio
	{
		using namespace ::asio;
	}
	namespace system
	{
		using ::std::error_code;
	}
}
#else
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/buffer.hpp>
#ifdef ZQ_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif

using tcp_socket = boost::asio::ip::tcp::socket;
#ifdef ZQ_ENABLE_SSL
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
#endif


#endif
