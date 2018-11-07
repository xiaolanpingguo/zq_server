#pragma once

#include <boost/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>
#if BOOST_VERSION >= 106600
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_context
#else
#include <boost/asio/io_service.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_service
#endif

namespace zq{

using tcp = boost::asio::ip::tcp;
namespace Asio{


class IoContext : public IoContextBaseNamespace::IoContextBase
{
    using IoContextBaseNamespace::IoContextBase::IoContextBase;
};

template<typename T>
inline decltype(auto) post(IoContextBaseNamespace::IoContextBase& ioContext, T&& t)
{
#if BOOST_VERSION >= 106600
    return boost::asio::post(ioContext, std::forward<T>(t));
#else
    return ioContext.post(std::forward<T>(t));
#endif
}

template<typename T>
inline decltype(auto) get_io_context(T&& ioObject)
{
#if BOOST_VERSION >= 106600
    return ioObject.get_executor().context();
#else
    return ioObject.get_io_service();
#endif
}

inline IoContext& getDefaultIoContextObj()
{
	static IoContext io_service(1);
	return io_service;
}

}
}

