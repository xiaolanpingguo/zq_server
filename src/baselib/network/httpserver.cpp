//#include <thread>
//#include "HttpServer.h"
//#include <sys/stat.h>
//
//#ifdef ENABLE_SSL
//
//#include <event2/bufferevent_ssl.h>
//#include <openssl/err.h>
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#pragma comment(lib, "libeay32.lib")
//#pragma comment(lib, "ssleay32.lib")
//#endif
//
//#endif
//
//using namespace zq;
//
//constexpr int MAX_HTTP_BUFF_LEN = 1024 * 1024 * 2;
//
//inline bool iequal(const char *s, size_t l, const char *t) 
//{
//	if (strlen(t) != l)
//		return false;
//
//	for (size_t i = 0; i < l; i++) {
//		if (std::tolower(s[i]) != std::tolower(t[i]))
//			return false;
//	}
//
//	return true;
//}
//
//HttpServer::HttpServer()
//	:
//	base_(nullptr)
//{
//
//}
//
//HttpServer::~HttpServer()
//{
//	if (base_)
//	{
//		event_base_free(base_);
//		base_ = nullptr;
//	}
//
//#ifdef ENABLE_SSL
//	if (sslCtx_)
//	{
//		SSL_CTX_free(sslCtx_);
//		sslCtx_ = nullptr;
//	}
//
//	EVP_cleanup();
//	ERR_free_strings();
//	ERR_remove_thread_state(NULL);
//	CRYPTO_cleanup_all_ex_data();
//	sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
//
//#endif
//}
//
//bool HttpServer::run()
//{
//    if (base_)
//    {
//        event_base_loop(base_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
//    }
//
//    return true;
//}
//
//int HttpServer::initServer(const unsigned short port, const std::string& crt_file, const std::string& key_file)
//{
//    struct evhttp* http;
//    struct evhttp_bound_socket* handle;
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//    WSADATA WSAData;
//    WSAStartup(0x101, &WSAData);
//#endif
//
//    base_ = event_base_new();
//    if (!base_)
//    {
//        return -2;
//    }
//
//    http = evhttp_new(base_);
//    if (!http)
//    {
//        return -3;
//    }
//#ifdef ENABLE_SSL
//
//	SSL_library_init();
//	ERR_load_crypto_strings();
//	SSL_load_error_strings();
//	OpenSSL_add_all_algorithms();
//
//	sslCtx_ = SSL_CTX_new(SSLv23_server_method());
//	if (!sslCtx_)
//	{
//		return -4;
//	}
//	SSL_CTX_set_options(sslCtx_, SSL_OP_SINGLE_DH_USE | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_SSLv2);
//
//	// 要求校验对方证书,如果不验证:SSL_VERIFY_NONE
//	SSL_CTX_set_verify(sslCtx_, SSL_VERIFY_PEER, NULL);
//
//	// 加载CA的证书
//	if (1 != SSL_CTX_load_verify_locations(sslCtx_, "ca/ca.crt", NULL))
//	{
//		return -5;
//	}
//
//	// 自己的证书
//	if (1 != SSL_CTX_use_certificate_chain_file(sslCtx_, "server/server.crt"))
//	{
//		return -5;
//	}
//
//	// 自己的私钥
//	if (1 != SSL_CTX_use_PrivateKey_file(sslCtx_, "server/server.key", SSL_FILETYPE_PEM))
//	{
//		return -6;
//	}
//	
//	// 私钥是否正确
//	if (!SSL_CTX_check_private_key(sslCtx_))
//	{
//		return -7;
//	}
//
//#endif
//			 
//    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
//    if (!handle)
//    {
//        return -4;
//    }
//
//#ifdef ENABLE_SSL
//	evhttp_set_bevcb(http, bevcb, (void*)sslCtx_);
//#endif
//    evhttp_set_gencb(http, listener_cb, (void*)this);
//	event_set_log_callback(log_cb);
//
//	multipartReader_.on_part_begin = std::bind(&HttpServer::onPartBeginCb, this, std::placeholders::_1);
//	multipartReader_.on_part_data = std::bind(&HttpServer::onPartDataCb, this, std::placeholders::_1, std::placeholders::_2);
//	multipartReader_.on_part_end = std::bind(&HttpServer::onPartEndCb, this);
//	multipartReader_.on_end = std::bind(&HttpServer::onPartDataEndCb, this);
//    return 0;
//}
//
//struct ::bufferevent* HttpServer::bevcb(struct event_base *base, void *arg)
//{
//#ifdef ENABLE_SSL
//	SSL_CTX *ctx = (SSL_CTX *)arg;
//	SSL* ssl = SSL_new(ctx);
//	return bufferevent_openssl_socket_new(base, -1, ssl, BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);
//#endif
//	return nullptr;
//}
//
//void HttpServer::log_cb(int severity, const char* data)
//{
//	std::cout << "severity: " << severity << ",log: " << data << std::endl;
//}
//
//void HttpServer::listener_cb(struct evhttp_request* req, void* arg)
//{
//	if (req == nullptr)
//	{
//		return;
//	}
//
//	HttpServer* pNet = (HttpServer*)arg;
//	if (pNet == NULL)
//	{
//		evhttp_send_error(req, HTTP_BADREQUEST, 0);
//		return;
//	}
//
//	stHttpRequest* request = pNet->allocateHttpRequest();
//	if (request == nullptr)
//	{
//		evhttp_send_error(req, HTTP_BADREQUEST, 0);
//		return;
//	}
//
//	request->req = req;
//
//	// headers
//	struct evkeyvalq* header = evhttp_request_get_input_headers(req);
//	if (header == nullptr)
//	{
//		pNet->httpRequestPool_.emplace_back(request);
//		evhttp_send_error(req, HTTP_BADREQUEST, 0);
//		return;
//	}
//
//	struct evkeyval* kv = header->tqh_first;
//	while (kv) 
//	{
//		request->headers.insert(std::make_pair(kv->key, kv->value));
//		kv = kv->next.tqe_next;
//	}
//
//    // uri
//    const char* uri = evhttp_request_get_uri(req);
//	if (uri == nullptr)
//	{
//		pNet->httpRequestPool_.emplace_back(request);
//		evhttp_send_error(req, HTTP_BADREQUEST, 0);
//		return;
//	}
//
//	request->url = uri;
//
//	// hostname
//	const char* hostname = evhttp_request_get_host(req);
//	if (hostname != nullptr)
//	{
//		//evhttp_find_vhost(http, &http, hostname);
//		request->remoteHost = hostname;
//	}
//
//    // decodeUri
//    struct evhttp_uri* decoded = evhttp_uri_parse(uri);
//    if (!decoded)
//    {
//		pNet->httpRequestPool_.emplace_back(request);
//        evhttp_send_error(req, HTTP_BADREQUEST, 0);
//        return;
//    }
//    
//	// path
//	const char* urlPath = evhttp_uri_get_path(decoded);
//	if (urlPath != nullptr)
//	{
//		request->path = urlPath;
//	}
//
//	// 记得释放
//	evhttp_uri_free(decoded);
//
//	request->type = (EnHttpType)evhttp_request_get_command(req);
//
//	// get
//	if (request->type == EnHttpType::EN_HTTP_REQ_GET)
//	{
//		struct evkeyvalq params;
//		evhttp_parse_query(uri, &params);
//		struct evkeyval* kv = params.tqh_first;
//		while (kv)
//		{
//			request->params.insert(std::make_pair(kv->key, kv->value));
//			kv = kv->next.tqe_next;
//		}
//	}
//	// post
//	else if (request->type == EnHttpType::EN_HTTP_REQ_POST)
//	{
//		struct evbuffer *in_evb = evhttp_request_get_input_buffer(req);
//		if (in_evb == nullptr)
//		{
//			pNet->httpRequestPool_.emplace_back(request);
//			return;
//		}
//
//		size_t len = evbuffer_get_length(in_evb);
//		if (len > 0)
//		{
//			unsigned char* pData = evbuffer_pullup(in_evb, len);
//			request->body.clear();
//
//			if (pData != nullptr)
//			{
//				request->body.append((const char*)pData, len);
//			}
//		}
//	}
//
//	if (request->type == EnHttpType::EN_HTTP_REQ_POST)
//	{
//		pNet->parsePost(*request);
//	}
//
//	if (pNet->filter_)
//	{
//		try
//		{
//			EnWebStatus xWebStatus = pNet->filter_(*request);
//			if (xWebStatus != EnWebStatus::WEB_OK)
//			{
//				pNet->httpRequestPool_.emplace_back(request);
//
//				// 401
//				pNet->responseMsg(*request, "Filter error", xWebStatus);
//				return;
//			}
//		}
//		catch (std::exception& e)
//		{
//			pNet->responseMsg(*request, e.what(), EnWebStatus::WEB_ERROR);
//		}
//		catch (...)
//		{
//			pNet->responseMsg(*request, "UNKNOW ERROR", EnWebStatus::WEB_ERROR);
//		}
//
//	}
//
//	// 回调
//	try
//	{
//		if (pNet->receiveCb_)
//		{
//			pNet->receiveCb_(*request);
//		}
//		else
//		{
//			pNet->responseMsg(*request, "NO PROCESSER", EnWebStatus::WEB_ERROR);
//		}
//	}
//	catch (std::exception& e)
//	{
//		pNet->responseMsg(*request, e.what(), EnWebStatus::WEB_ERROR);
//	}
//	catch (...)
//	{
//		pNet->responseMsg(*request, "UNKNOW ERROR", EnWebStatus::WEB_ERROR);
//	}
//}
//
//bool HttpServer::responseMsg(const stHttpRequest& req, const std::string& strMsg, EnWebStatus code, const std::string& strReason)
//{
//	auto it = httpRequestMap_.find(req.id);
//	if (it != httpRequestMap_.end())
//	{
//		httpRequestPool_.emplace_back(it->second);
//		httpRequestMap_.erase(it);
//	}
//
//    evhttp_request* pHttpReq = (evhttp_request*)req.req;
//    struct evbuffer* eventBuffer = evbuffer_new();
//
//    evbuffer_add_printf(eventBuffer, strMsg.c_str());
//    evhttp_add_header(evhttp_request_get_output_headers(pHttpReq), "Content-Type", "application/json");
//    evhttp_send_reply(pHttpReq, code, strReason.c_str(), eventBuffer);
//
//	// 记得释放
//    evbuffer_free(eventBuffer);
//
//    return true;
//}
//
//stHttpRequest* HttpServer::allocateHttpRequest()
//{
//	static const int pool_size = 64;
//	if (httpRequestPool_.empty())
//	{
//		for (int i = 0; i < pool_size; ++i)
//		{
//			httpRequestPool_.emplace_back(new stHttpRequest());
//		}
//	}
//
//	stHttpRequest* pRequest = httpRequestPool_.front();
//	httpRequestPool_.pop_front();
//	pRequest->reset();
//
//	pRequest->id = ++index_;
//
//	return pRequest;
//}
//
//bool HttpServer::parsePost(const stHttpRequest& req)
//{
//	EnContentTypeReq type = getContentType(req);
//
//	switch (type)
//	{
//	case EnContentTypeReq::EN_STRING:
//	case EnContentTypeReq::EN_UNKNOWN:
//		return handleStringBody(req);
//		break;
//	case EnContentTypeReq::EN_MULTIPART:
//		return handleMultipart(req);
//		break;
//	case EnContentTypeReq::EN_OCTET_STREAM:
//		return handleCctetStream(req);
//		break;
//	case EnContentTypeReq::EN_URLENCODE:
//		return handleFormUrlencoded(req);
//		break;
//	case EnContentTypeReq::EN_CHUNKED:
//		return handleChunked(req);
//		break;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//EnContentTypeReq HttpServer::getContentType(const stHttpRequest& req)
//{
//	std::string content_type = "";
//	for (auto it = req.headers.begin(); it != req.headers.end(); ++it)
//	{
//		if (iequal(it->first.c_str(), it->first.size(), "content-type"))
//		{
//			content_type = it->second;
//			break;
//		}
//	}
//	if (content_type.empty())
//	{
//		return EnContentTypeReq::EN_UNKNOWN;
//	}
//	else
//	{
//		if (content_type.find("application/x-www-form-urlencoded") != std::string::npos)
//		{
//			return EnContentTypeReq::EN_URLENCODE;
//		}
//		else if (content_type.find("multipart/form-data") != std::string::npos)
//		{
//			size_t size = content_type.find("=");
//			if (size == std::string::npos)
//			{
//				return EnContentTypeReq::EN_UNKNOWN;
//			}
//
//			auto bd = content_type.substr(size + 1, content_type.length() - size);
//			std::string boundary(bd.data(), bd.length());
//			multipartReader_.set_boundary("\r\n--" + std::move(boundary));
//			return EnContentTypeReq::EN_MULTIPART;
//		}
//		else if (content_type.find("application/octet-stream") != std::string::npos)
//		{
//			return EnContentTypeReq::EN_OCTET_STREAM;
//		}
//		else
//		{
//			return EnContentTypeReq::EN_STRING;
//		}
//	}
//}
//
//bool HttpServer::handleStringBody(const stHttpRequest& req)
//{
//	return false;
//}
//
//bool HttpServer::handleMultipart(const stHttpRequest& req)
//{
//	const std::string& body = req.body;
//	if (body.empty())
//	{
//		return false;
//	}
//
//	size_t bufsize = body.length();
//	size_t fed = 0;
//
//	try 
//	{
//		do
//		{
//			size_t ret = multipartReader_.feed(body.data() + fed, body.length() - fed);
//			fed += ret;
//		} while (fed < bufsize && !multipartReader_.stopped());
//	}
//	catch (...)
//	{
//		return false;
//	}
//
//	if (multipartReader_.has_error())
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool HttpServer::handleCctetStream(const stHttpRequest& req)
//{
//	return false;
//}
//
//bool HttpServer::handleFormUrlencoded(const stHttpRequest& req)
//{
//	return false;
//}
//
//bool HttpServer::handleChunked(const stHttpRequest& req)
//{
//	return false;
//}
//
//void HttpServer::onPartBeginCb(const multipart_headers& headers)
//{
//
//}
//
//void HttpServer::onPartDataCb(const char* data, size_t len)
//{
//	//std::string aa(data, len);
//}
//
//void HttpServer::onPartEndCb()
//{
//
//}
//
//void HttpServer::onPartDataEndCb()
//{
//
//}