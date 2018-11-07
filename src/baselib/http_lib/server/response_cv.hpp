#pragma once


#include <string>

namespace zq {


enum class content_type
{
	string,
	multipart,
	urlencoded,
	chunked,
	octet_stream,
	websocket,
	unknown,
};

enum class res_content_type
{
	html,
	json,
	string,
	none
};

enum class status_type 
{
	init,
	switching_protocols = 101,
	ok = 200,
	created = 201,
	accepted = 202,
	no_content = 204,
	partial_content = 206,
	multiple_choices = 300,
	moved_permanently = 301,
	moved_temporarily = 302,
	not_modified = 304,
	bad_request = 400,
	unauthorized = 401,
	forbidden = 403,
	not_found = 404,
	internal_server_error = 500,
	not_implemented = 501,
	bad_gateway = 502,
	service_unavailable = 503
};

enum class content_encoding 
{
	gzip,
	none
};

enum class http_method
{
	DEL,
	GET,
	HEAD,
	POST,
	PUT,
	CONNECT,
	OPTIONS,
	TRACE,
	UNKNOWN
};

static constexpr auto GET = http_method::GET;
static constexpr auto POST = http_method::POST;
static constexpr auto DEL = http_method::DEL;
static constexpr auto HEAD = http_method::HEAD;
static constexpr auto PUT = http_method::PUT;
static constexpr auto CONNECT = http_method::CONNECT;
static constexpr auto TRACE = http_method::TRACE;
static constexpr auto OPTIONS = http_method::OPTIONS;
static constexpr auto UNKNOWN = http_method::UNKNOWN;

inline http_method getMethodByStr(const std::string& str)
{
	if (str == "get" || str == "GET")
		return GET;
	if (str == "post" || str == "POST")
		return POST;
	if (str == "del" || str == "DEL")
		return DEL;
	if (str == "head" || str == "UNKNOWN")
		return HEAD;
	if (str == "put" || str == "PUT")
		return PUT;
	if (str == "connect" || str == "CONNECT")
		return CONNECT;
	if (str == "trace" || str == "TRACE")
		return TRACE;
	if (str == "options" || str == "OPTIONS")
		return OPTIONS;
	else
		return UNKNOWN;
}


static std::map<zq::res_content_type, std::string> res_mime_map =
{
	{ res_content_type::html, "text/html; charset=utf8" },
	{ res_content_type::json, "application/json; charset=utf8" },
	{ res_content_type::string, "text/plain; charset=utf8" }
};


static const std::string ok = "OK";
static const std::string created = "<html>"
	"<head><title>Created</title></head>"
	"<body><h1>201 Created</h1></body>"
	"</html>";

static const std::string accepted =
	"<html>"
	"<head><title>Accepted</title></head>"
	"<body><h1>202 Accepted</h1></body>"
	"</html>";

static const std::string no_content =
	"<html>"
	"<head><title>No Content</title></head>"
	"<body><h1>204 Content</h1></body>"
	"</html>";

static const std::string multiple_choices =
	"<html>"
	"<head><title>Multiple Choices</title></head>"
	"<body><h1>300 Multiple Choices</h1></body>"
	"</html>";

static const std::string moved_permanently =
	"<html>"
	"<head><title>Moved Permanently</title></head>"
	"<body><h1>301 Moved Permanently</h1></body>"
	"</html>";

static const std::string moved_temporarily =
	"<html>"
	"<head><title>Moved Temporarily</title></head>"
	"<body><h1>302 Moved Temporarily</h1></body>"
	"</html>";

static const std::string not_modified =
	"<html>"
	"<head><title>Not Modified</title></head>"
	"<body><h1>304 Not Modified</h1></body>"
	"</html>";

static const std::string bad_request =
	"<html>"
	"<head><title>Bad Request</title></head>"
	"<body><h1>400 Bad Request</h1></body>"
	"</html>";

static const std::string unauthorized =
	"<html>"
	"<head><title>Unauthorized</title></head>"
	"<body><h1>401 Unauthorized</h1></body>"
	"</html>";

static const std::string forbidden =
	"<html>"
	"<head><title>Forbidden</title></head>"
	"<body><h1>403 Forbidden</h1></body>"
	"</html>";

static const std::string not_found =
	"<html>"
	"<head><title>Not Found</title></head>"
	"<body><h1>404 Not Found</h1></body>"
	"</html>";

static const std::string internal_server_error =
	"<html>"
	"<head><title>Internal Server Error</title></head>"
	"<body><h1>500 Internal Server Error</h1></body>"
	"</html>";

static const std::string not_implemented =
	"<html>"
	"<head><title>Not Implemented</title></head>"
	"<body><h1>501 Not Implemented</h1></body>"
	"</html>";

static const std::string bad_gateway =
	"<html>"
	"<head><title>Bad Gateway</title></head>"
	"<body><h1>502 Bad Gateway</h1></body>"
	"</html>";

static const std::string service_unavailable =
	"<html>"
	"<head><title>Service Unavailable</title></head>"
	"<body><h1>503 Service Unavailable</h1></body>"
	"</html>";

static const std::string switching_protocols = "HTTP/1.1 101 Switching Protocals\r\n";
static const std::string rep_ok = "HTTP/1.1 200 OK\r\n";
static const std::string rep_created = "HTTP/1.1 201 Created\r\n";
static const std::string rep_accepted = "HTTP/1.1 202 Accepted\r\n";
static const std::string rep_no_content = "HTTP/1.1 204 No Content\r\n";
static const std::string rep_partial_content = "HTTP/1.1 206 Partial Content\r\n";
static const std::string rep_multiple_choices = "HTTP/1.1 300 Multiple Choices\r\n";
static const std::string rep_moved_permanently =	"HTTP/1.1 301 Moved Permanently\r\n";
static const std::string rep_moved_temporarily =	"HTTP/1.1 302 Moved Temporarily\r\n";
static const std::string rep_not_modified = "HTTP/1.1 304 Not Modified\r\n";
static const std::string rep_bad_request = "HTTP/1.1 400 Bad Request\r\n";
static const std::string rep_unauthorized = "HTTP/1.1 401 Unauthorized\r\n";
static const std::string rep_forbidden =	"HTTP/1.1 403 Forbidden\r\n";
static const std::string rep_not_found =	"HTTP/1.1 404 Not Found\r\n";
static const std::string rep_internal_server_error = "HTTP/1.1 500 Internal Server Error\r\n";
static const std::string rep_not_implemented = "HTTP/1.1 501 Not Implemented\r\n";
static const std::string rep_bad_gateway = "HTTP/1.1 502 Bad Gateway\r\n";
static const std::string rep_service_unavailable = "HTTP/1.1 503 Service Unavailable\r\n";

static const char name_value_separator[] = { ':', ' ' };
//static const std::string crlf = "\r\n";

static const char crlf[] = { '\r', '\n' };
static const char last_chunk[] = { '0', '\r', '\n' };
static const std::string http_chunk_header =
	"HTTP/1.1 200 OK\r\n"
	"Transfer-Encoding: chunked\r\n";
	/*"Content-Type: video/mp4\r\n"
	"\r\n";*/

static const std::string http_range_chunk_header =
			"HTTP/1.1 206 Partial Content\r\n"
			"Transfer-Encoding: chunked\r\n";
			/*"Content-Type: video/mp4\r\n"
			"\r\n";*/

static inline boost::asio::const_buffer to_buffer(status_type status) 
{
	switch (status) 
	{
	case status_type::switching_protocols:
		return boost::asio::buffer(switching_protocols.data(), switching_protocols.length());
	case status_type::ok:
		return boost::asio::buffer(rep_ok.data(), rep_ok.length());
	case status_type::created:
		return boost::asio::buffer(rep_created.data(), rep_created.length());
	case status_type::accepted:
		return boost::asio::buffer(rep_accepted.data(), rep_created.length());
	case status_type::no_content:
		return boost::asio::buffer(rep_no_content.data(), rep_no_content.length());
	case status_type::partial_content:
		return boost::asio::buffer(rep_partial_content.data(), rep_partial_content.length());
	case status_type::multiple_choices:
		return boost::asio::buffer(rep_multiple_choices.data(), rep_multiple_choices.length());
	case status_type::moved_permanently:
		return boost::asio::buffer(rep_moved_permanently.data(), rep_moved_permanently.length());
	case status_type::moved_temporarily:
		return boost::asio::buffer(rep_moved_temporarily.data(), rep_moved_temporarily.length());
	case status_type::not_modified:
		return boost::asio::buffer(rep_not_modified.data(), rep_not_modified.length());
	case status_type::bad_request:
		return boost::asio::buffer(rep_bad_request.data(), rep_bad_request.length());
	case status_type::unauthorized:
		return boost::asio::buffer(rep_unauthorized.data(), rep_unauthorized.length());
	case status_type::forbidden:
		return boost::asio::buffer(rep_forbidden.data(), rep_forbidden.length());
	case status_type::not_found:
		return boost::asio::buffer(rep_not_found.data(), rep_not_found.length());
	case status_type::internal_server_error:
		return boost::asio::buffer(rep_internal_server_error.data(), rep_internal_server_error.length());
	case status_type::not_implemented:
		return boost::asio::buffer(rep_not_implemented.data(), rep_not_implemented.length());
	case status_type::bad_gateway:
		return boost::asio::buffer(rep_bad_gateway.data(), rep_bad_gateway.length());
	case status_type::service_unavailable:
		return boost::asio::buffer(rep_service_unavailable.data(), rep_service_unavailable.length());
	default:
		return boost::asio::buffer(rep_internal_server_error.data(), rep_internal_server_error.length());
	}
}

static inline const std::string& to_string(status_type status) 
{
	switch (status) {
	case status_type::ok:
		return ok;
	case status_type::created:
		return created;
	case status_type::accepted:
		return accepted;
	case status_type::no_content:
		return no_content;
	case status_type::multiple_choices:
		return multiple_choices;
	case status_type::moved_permanently:
		return moved_permanently;
	case status_type::moved_temporarily:
		return moved_temporarily;
	case status_type::not_modified:
		return not_modified;
	case status_type::bad_request:
		return bad_request;
	case status_type::unauthorized:
		return unauthorized;
	case status_type::forbidden:
		return forbidden;
	case status_type::not_found:
		return not_found;
	case status_type::internal_server_error:
		return internal_server_error;
	case status_type::not_implemented:
		return not_implemented;
	case status_type::bad_gateway:
		return bad_gateway;
	case status_type::service_unavailable:
		return service_unavailable;
	default:
		return internal_server_error;
	}
}



}

