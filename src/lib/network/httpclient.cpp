//#include "httpclient.h"
//
//#ifdef ENABLE_SSL
//
//#include <event2/bufferevent_ssl.h>
//#include <openssl/err.h>
//#include <openssl/rand.h>
//#include <openssl/x509v3.h>
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#pragma comment(lib, "libeay32.lib")
//#pragma comment(lib, "ssleay32.lib")
//#endif
//
//#endif
//
//namespace zq {
//
//#ifdef ENABLE_SSL
//
//#define HOSTNAME_MAX_SIZE 255
//
//typedef enum 
//{
//	MatchFound,
//	MatchNotFound,
//	NoSANPresent,
//	MalformedCertificate,
//	Error
//} HostnameValidationResult;
//
///**
//* Tries to find a match for hostname in the certificate's Common Name field.
//*
//* Returns MatchFound if a match was found.
//* Returns MatchNotFound if no matches were found.
//* Returns MalformedCertificate if the Common Name had a NUL character embedded in it.
//* Returns Error if the Common Name could not be extracted.
//*/
//static HostnameValidationResult matches_common_name(const char *hostname, const X509 *server_cert) 
//{
//	int common_name_loc = -1;
//	X509_NAME_ENTRY *common_name_entry = NULL;
//	ASN1_STRING *common_name_asn1 = NULL;
//	char *common_name_str = NULL;
//
//	// Find the position of the CN field in the Subject field of the certificate
//	common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name((X509 *)server_cert), NID_commonName, -1);
//	if (common_name_loc < 0) 
//	{
//		return Error;
//	}
//
//	// Extract the CN field
//	common_name_entry = X509_NAME_get_entry(X509_get_subject_name((X509 *)server_cert), common_name_loc);
//	if (common_name_entry == NULL) 
//	{
//		return Error;
//	}
//
//	// Convert the CN field to a C string
//	common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
//	if (common_name_asn1 == NULL) 
//	{
//		return Error;
//	}
//	common_name_str = (char *)ASN1_STRING_data(common_name_asn1);
//
//	// Make sure there isn't an embedded NUL character in the CN
//	if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
//		return MalformedCertificate;
//	}
//
//	// Compare expected hostname with the CN
//#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
//	if (strcasecmp(hostname, common_name_str) == 0) 
//	{
//		return MatchFound;
//	}
//#else
//	if (stricmp(hostname, common_name_str) == 0)
//	{
//		return MatchFound;
//	}
//#endif
//	else 
//	{
//		return MatchNotFound;
//	}
//}
//
//
///**
//* Tries to find a match for hostname in the certificate's Subject Alternative Name extension.
//*
//* Returns MatchFound if a match was found.
//* Returns MatchNotFound if no matches were found.
//* Returns MalformedCertificate if any of the hostnames had a NUL character embedded in it.
//* Returns NoSANPresent if the SAN extension was not present in the certificate.
//*/
//static HostnameValidationResult matches_subject_alternative_name(const char *hostname, const X509 *server_cert) 
//{
//	HostnameValidationResult result = MatchNotFound;
//	int i;
//	int san_names_nb = -1;
//	STACK_OF(GENERAL_NAME)* san_names = NULL;
//
//	// Try to extract the names within the SAN extension from the certificate
//	san_names = (STACK_OF(GENERAL_NAME)*)X509_get_ext_d2i((X509 *)server_cert, NID_subject_alt_name, NULL, NULL);
//	if (san_names == NULL) 
//	{
//		return NoSANPresent;
//	}
//	san_names_nb = sk_GENERAL_NAME_num(san_names);
//
//	// Check each name within the extension
//	for (i = 0; i < san_names_nb; i++) 
//	{
//		const GENERAL_NAME *current_name = sk_GENERAL_NAME_value(san_names, i);
//
//		if (current_name->type == GEN_DNS) 
//		{
//			// Current name is a DNS name, let's check it
//			char *dns_name = (char *)ASN1_STRING_data(current_name->d.dNSName);
//
//			// Make sure there isn't an embedded NUL character in the DNS name
//			if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
//				result = MalformedCertificate;
//				break;
//			}
//			else 
//			{ 
//#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX			
//				// Compare expected hostname with the DNS name
//				if (strcasecmp(hostname, dns_name) == 0) 
//				{
//					result = MatchFound;
//					break;
//				}
//#else	
//				if (stricmp(hostname, dns_name) == 0)
//				{
//					result = MatchFound;
//					break;
//				}
//#endif
//			}
//		}
//	}
//	sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);
//
//	return result;
//}
//
//
//static HostnameValidationResult validate_hostname(const char *hostname, const X509 *server_cert)
//{
//	HostnameValidationResult result;
//
//	if ((hostname == NULL) || (server_cert == NULL))
//		return Error;
//
//	// First try the Subject Alternative Names extension
//	result = matches_subject_alternative_name(hostname, server_cert);
//	if (result == NoSANPresent) {
//		// Extension was not found: try the Common Name
//		result = matches_common_name(hostname, server_cert);
//	}
//
//	return result;
//}
//
//static int cert_verify_callback(X509_STORE_CTX *x509_ctx, void *arg)
//{
//	// 验证证书的回调，验证该地址是否合法，
//	// 这里，我在本机测试了下发现证书老是通不过，所以这里
//	// 我们客户端不用验证，如果要验证的话，在之前初始化函数添加回调即可：
//	// SSL_CTX_set_cert_verify_callback(sslCtx_, cert_verify_callback, (void*)host);
//	const char *host = (const char *)arg;
//	const char *res_str = "X509_verify_cert failed";
//	HostnameValidationResult res = Error;
//
//	/* This is the function that OpenSSL would call if we hadn't called
//	* SSL_CTX_set_cert_verify_callback().  Therefore, we are "wrapping"
//	* the default functionality, rather than replacing it. */
//	int ok_so_far = X509_verify_cert(x509_ctx);
//
//	X509 *server_cert = X509_STORE_CTX_get_current_cert(x509_ctx);
//
//	if (ok_so_far)
//	{
//		res = validate_hostname(host, server_cert);
//
//		switch (res)
//		{
//		case MatchFound:
//			res_str = "MatchFound";
//			break;
//		case MatchNotFound:
//			res_str = "MatchNotFound";
//			break;
//		case NoSANPresent:
//			res_str = "NoSANPresent";
//			break;
//		case MalformedCertificate:
//			res_str = "MalformedCertificate";
//			break;
//		case Error:
//			res_str = "Error";
//			break;
//		default:
//			res_str = "WTF!";
//			break;
//		}
//	}
//
//	char cert_str[256];
//	X509_NAME_oneline(X509_get_subject_name(server_cert),
//		cert_str, sizeof(cert_str));
//
//	if (res == MatchFound)
//	{
//		printf("https server '%s' has this certificate, "
//			"which looks good to me:\n%s\n",
//			host, cert_str);
//		return 1;
//	}
//	else
//	{
//		printf("Got '%s' for hostname '%s' and certificate:\n%s\n",
//			res_str, host, cert_str);
//		return 0;
//	}
//}
//
//#endif
//
//HttpClient::HttpClient(int nRetry, int nTimeoutSec)
//	: retry_(nRetry), timeOut_(nTimeoutSec)
//{
//}
//
//HttpClient::~HttpClient()
//{
//	for (auto it = listHttpObjects_.begin(); it != listHttpObjects_.end(); ++it)
//	{
//		if (*it)
//		{
//			delete (*it);
//			*it = nullptr;
//		}
//	}
//
//	listHttpObjects_.clear();
//
//	if (eventBase_)
//	{
//		event_base_free(eventBase_);
//		eventBase_ = nullptr;
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
//bool HttpClient::run()
//{
//	if (eventBase_)
//	{
//		event_base_loop(eventBase_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
//	}
//
//	return true;
//}
//
//bool HttpClient::init()
//{
//	eventBase_ = event_base_new();
//	if (eventBase_ == nullptr)
//	{
//		return false;
//	}
//
//	constexpr static int pool_size = 1024;
//	for (int i = 0; i < pool_size; ++i)
//	{
//		listHttpObjects_.push_back(new HttpObject(this, nullptr, nullptr, Guid()));
//	}
//
//#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//	WORD wVersionRequested;
//	WSADATA wsaData;
//	wVersionRequested = MAKEWORD(2, 2);
//	int err = WSAStartup(wVersionRequested, &wsaData);
//	if (err != 0)
//	{
//		return false;
//	}
//#endif
//
//#ifdef ENABLE_SSL
//	SSL_library_init();
//	ERR_load_crypto_strings();
//	SSL_load_error_strings();
//	OpenSSL_add_all_algorithms();
//
//	sslCtx_ = SSL_CTX_new(SSLv23_client_method());
//	if (!sslCtx_)
//	{
//		return false;
//	}
//
//	int r = RAND_poll();
//	if (r == 0)
//	{
//		return false;
//	}
//
//	// 加载CA的证书
//	if (!SSL_CTX_load_verify_locations(sslCtx_, "ca/ca.crt", NULL))
//	{
//		return false;
//	}
//
//	// 要求校验对方证书,表示需要验证服务器端,若不需要验证则使用:SSL_VERIFY_NONE
//	SSL_CTX_set_verify(sslCtx_, SSL_VERIFY_NONE, NULL);
//
//	// 自己的证书
//	if (1 != SSL_CTX_use_certificate_chain_file(sslCtx_, "client/client.crt"))
//	{
//		return false;
//	}
//
//	//// 自己的私钥,加载私钥需要密码,意思是每次链接服务器都需要密码
//	//// 若服务器需要验证客户端的身份,则需要客户端加载私钥
//	//// 由于此处我们只需要验证服务器身份，故无需加载自己的私钥
//	//if (1 != SSL_CTX_use_PrivateKey_file(sslCtx_, "client/client.key", SSL_FILETYPE_PEM))
//	//{
//	//	return false;
//	//}
//
//	//// 私钥是否正确
//	//if (!SSL_CTX_check_private_key(sslCtx_))
//	//{
//	//	return false;
//	//}
//
//#endif
//
//	return true;
//}
//
//bool HttpClient::makeRequest(const std::string& strUri,
//	HTTP_RESP_FUNCTOR&& cb,
//	const std::string& strPostData,
//	const std::map<std::string, std::string>& headers,
//	const EnHttpType eHttpType,
//	const Guid id)
//{
//	struct evhttp_uri* http_uri = evhttp_uri_parse(strUri.c_str());
//	if (http_uri == nullptr)
//	{
//		return false;
//	}
//
//	const char* scheme = evhttp_uri_get_scheme(http_uri);
//	if (scheme == nullptr)
//	{
//		return false;
//	}
//
//	std::string lowwerScheme(scheme);
//	std::transform(lowwerScheme.begin(), lowwerScheme.end(), lowwerScheme.begin(), (int(*)(int)) std::tolower);
//	if (lowwerScheme.compare("https") != 0 && lowwerScheme.compare("http") != 0)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//	bool isHttps = false;
//	if (lowwerScheme.compare("https") == 0)
//	{
//		isHttps = true;
//	}
//
//	const char* host = evhttp_uri_get_host(http_uri);
//	if (host == nullptr)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//	int port = evhttp_uri_get_port(http_uri);
//	if (port == -1)
//	{
//		port = isHttps ? 443 : 80;
//	}
//
//	const char* path = evhttp_uri_get_path(http_uri);
//	if (path == nullptr)
//	{
//		return false;
//	}
//
//	if (strlen(path) == 0)
//	{
//		path = "/";
//	}
//
//	std::string uri;
//	const char* query = evhttp_uri_get_query(http_uri);
//	if (query == nullptr)
//	{
//		uri = path;
//	}
//	else
//	{
//		uri += path;
//		uri += "?";
//		uri += query;
//	}
//
//	struct bufferevent* bev = nullptr;
//
//#ifdef ENABLE_SSL
//
//	SSL* ssl = SSL_new(sslCtx_);
//	if (ssl == NULL)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//#endif
//
//	if (!isHttps)
//	{
//		bev = bufferevent_socket_new(eventBase_, -1, BEV_OPT_CLOSE_ON_FREE);
//	}
//	else
//	{
//#ifdef ENABLE_SSL
//		bev = bufferevent_openssl_socket_new(eventBase_, -1, ssl, BUFFEREVENT_SSL_CONNECTING,
//			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
//#endif
//	}
//
//	if (bev == nullptr)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//#ifdef ENABLE_SSL
//	if (isHttps)
//	{
//		bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);
//	}
//#endif
//
//	struct evhttp_connection* evcon = evhttp_connection_base_bufferevent_new(eventBase_, NULL, bev, host, port);
//	if (evcon == nullptr)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//	if (retry_ > 0)
//	{
//		evhttp_connection_set_retries(evcon, retry_);
//	}
//	if (timeOut_ >= 0)
//	{
//		evhttp_connection_set_timeout(evcon, timeOut_);
//	}
//
//	HttpObject* pHttpObj = nullptr;
//	if (listHttpObjects_.size() > 0)
//	{
//		pHttpObj = listHttpObjects_.front();
//
//		pHttpObj->httpClient_ = this;
//		pHttpObj->bev_ = bev;
//		pHttpObj->cb_ = std::move(cb);
//		pHttpObj->guid_ = id;
//	}
//	else
//	{
//		pHttpObj = new HttpObject(this, bev, std::move(cb), id);
//	}
//
//	if (pHttpObj == nullptr)
//	{
//		return false;
//	}
//
//	struct evhttp_request* req = evhttp_request_new(onHttpReqDone, pHttpObj);
//	if (req == nullptr)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//	struct evkeyvalq* output_headers = evhttp_request_get_output_headers(req);
//	if (output_headers == nullptr)
//	{
//		return false;
//	}
//
//	evhttp_add_header(output_headers, "Host", host);
//	evhttp_add_header(output_headers, "Connection", "close");
//	//evhttp_add_header(output_headers, "Connection", "keep-alive");
//
//	for (auto it = headers.cbegin(); it != headers.cend(); ++it)
//	{
//		evhttp_add_header(output_headers, it->first.c_str(), it->second.c_str());
//	}
//
//	size_t nLen = strPostData.length();
//	if (nLen > 0)
//	{
//		struct evbuffer* output_buffer = evhttp_request_get_output_buffer(req);
//		if (output_buffer == NULL)
//		{
//			return false;
//		}
//
//		evbuffer_add(output_buffer, strPostData.c_str(), nLen);
//		char buf[256] = { 0 };
//		evutil_snprintf(buf, sizeof(buf) - 1, "%lu", (unsigned long)nLen);
//		evhttp_add_header(output_headers, "Content-Length", buf);
//	}
//
//	int r_ = evhttp_make_request(evcon, req, (evhttp_cmd_type)eHttpType, uri.c_str());
//	if (r_ != 0)
//	{
//		if (http_uri)
//		{
//			evhttp_uri_free(http_uri);
//		}
//
//		return false;
//	}
//
//	if (http_uri)
//	{
//		evhttp_uri_free(http_uri);
//	}
//
//	return true;
//}
//
//bool HttpClient::doGet(const std::string& strUri, HTTP_RESP_FUNCTOR&& cb,
//	const std::map<std::string, std::string>& headers, const Guid id)
//{
//	return makeRequest(strUri, std::move(cb), "", headers, EnHttpType::EN_HTTP_REQ_GET, id);
//}
//
//bool HttpClient::doPost(const std::string& strUri, const std::string& strPostData, HTTP_RESP_FUNCTOR&& cb,
//	const std::map<std::string, std::string>& headers, const Guid id)
//{
//	return makeRequest(strUri, std::move(cb), strPostData, headers, EnHttpType::EN_HTTP_REQ_POST, id);
//}
//
//void HttpClient::onHttpReqDone(struct evhttp_request* req, void* ctx)
//{
//	HttpObject* pHttpObj = (HttpObject*)ctx;
//	if (pHttpObj == nullptr)
//	{
//		return;
//	}
//
//	if (req == nullptr)
//	{
//		/* If req is NULL, it means an error occurred, but
//		* sadly we are mostly left guessing what the error
//		* might have been.  We'll do our best... */
//		int printed_err = 0;
//		int errcode = EVUTIL_SOCKET_ERROR();
//
//		std::string strErrMsg = "";
//		/* Print out the OpenSSL error queue that libevent
//		* squirreled away for us, if any. */
//
//#ifdef ENABLE_SSL
//		char buffer[1024] = { 0 };
//		struct ::bufferevent* bev = (struct ::bufferevent*)pHttpObj->bev_;
//		unsigned long oslerr = 0;
//		while ((oslerr = bufferevent_get_openssl_error(bev)))
//		{
//			ERR_error_string_n(oslerr, buffer, sizeof(buffer));
//			fprintf(stderr, "%s\n", buffer);
//			strErrMsg += std::string(buffer);
//			printed_err = 1;
//		}
//#endif
//		/* If the OpenSSL error queue was empty, maybe it was a
//		* socket error; let's try printing that. */
//		if (!printed_err)
//		{
//			char tmpBuf[1024] = { 0 };
//			snprintf(tmpBuf, sizeof(tmpBuf), "socket error = %s (%d)\n",
//				evutil_socket_error_to_string(errcode),
//				errcode);
//			strErrMsg += std::string(tmpBuf);
//		}
//
//		fprintf(stderr, "http requst, error occurred,error: %s\n", strErrMsg.c_str());
//
//		HttpClient* pHttpClient = (HttpClient*)(pHttpObj->httpClient_);
//		pHttpClient->listHttpObjects_.emplace_back(pHttpObj);
//
//		return;
//	}
//
//	int nRespCode = evhttp_request_get_response_code(req);
//	char buffer[512] = { 0 };
//	int nread = 0;
//	std::string strResp;
//	while ((nread = evbuffer_remove(evhttp_request_get_input_buffer(req),
//		buffer, sizeof(buffer)))
//		> 0)
//	{
//		//TODO it's not good idea,to append or memcpy
//		strResp += std::string(buffer, nread);
//	}
//
//	if (req->evcon)
//	{
//		evhttp_connection_free(req->evcon);
//	}
//
//	if (req->output_headers)
//	{
//		evhttp_clear_headers(req->output_headers);
//	}
//
//	if (pHttpObj)
//	{
//		if (pHttpObj->bev_)
//		{
//#ifdef ENABLE_SSL
//			SSL* pSSL = bufferevent_openssl_get_ssl(pHttpObj->bev_);
//			SSL_free(pSSL);
//#endif
//		}
//	}
//
//	if (pHttpObj->cb_)
//	{
//		try
//		{
//			(pHttpObj->cb_)(pHttpObj->guid_, nRespCode, strResp);
//		}
//		catch (...)
//		{
//		}
//	}
//}
//}
