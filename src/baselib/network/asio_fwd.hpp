#pragma once


// ÓÃ¶ÀÁ¢°æµÄasio
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#include <asio.hpp>

namespace zq{


using tcp_t = asio::ip::tcp;
using io_context_t = asio::io_context;
using address_t = asio::ip::address;
using sys_err_t = asio::system_error;
using error_code_t = asio::error_code;
inline io_context_t& getDefaultIoContextObj()
{
	static io_context_t io_service(1);
	return io_service;
}

}


