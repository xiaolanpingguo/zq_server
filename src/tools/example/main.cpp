#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <stdarg.h>
#include <type_traits>
#include <tuple>
#include <array>
#include <sstream>
#include <thread>
#include <boost/coroutine2/all.hpp>
#include "baselib/libloader/coroutine_manager.h"
////#include "CoroBehaviour.h"

//#include "baselib/interface_header/platform.h"
//#include <google/protobuf/message.h>
//#include "baselib/message/test.pb.h"
//#include "baselib/core/map_ex.hpp"
//#include "dependencies/ajson/ajson.hpp"
//#include "baselib/db_redis/redis_module.h"
//#include "baselib/kernel/timer_module.h"
//#include "baselib/libloader/libmanager.h"
//#include "baselib/network/httpclient_module.h"
//#include "baselib/db_mysql/mysql_module.h"
//#include "baselib/db_mysql/database_loader.h"
//#include "baselib/db_mysql/query_callback_processor.h"


#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
//#include <WinSock2.h>
#endif

//#include <openssl/bio.h> 
//#include <openssl/ssl.h> 
//#include <openssl/err.h> 

//#include <event2/bufferevent.h>
//#include <event2/buffer.h>
//#include <event2/listener.h>
//#include <event2/util.h>
//#include <event2/thread.h>
//#include <event2/event_compat.h>
//#include <event2/bufferevent_struct.h>
//#include <event2/event.h>


#if _WIN32	
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ws2_32.lib")	 
#pragma comment(lib, "libprotobuf_d.lib")	
#pragma comment(lib, "libprotobuf_d.lib")	
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "base_code_d.lib")
#pragma comment(lib, "kernel_d.lib")
#pragma comment(lib, "core_d.lib")
#pragma comment(lib, "config_d.lib")
#pragma comment(lib, "network_d.lib")
#pragma comment(lib, "log_d.lib")
#pragma comment(lib, "message_d.lib")
#pragma comment(lib, "component_d.lib")
#pragma comment(lib, "db_redis_d.lib")
#pragma comment(lib, "db_mysql_d.lib")
#pragma comment(lib, "master_server_d.lib")
#pragma comment(lib, "world_server_d.lib")
#pragma comment(lib, "login_server_d.lib")
#pragma comment(lib, "game_server_d.lib")
#pragma comment(lib, "gate_server_d.lib")
#pragma comment(lib, "libloader_d.lib")
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

using namespace std;
using namespace zq;


//int test_ssl_server()
//{
//	static constexpr short SERVER_PORT = 31001;
//	static constexpr const char* CA_CERT_FILE = "ca/ca.crt";
//	static constexpr const char* SERVER_CERT_FILE = "server/server.crt";
//	static constexpr const char* SERVER_KEY_FILE = "server/server.key";
//
//#ifdef WIN32
//	//windows初始化网络环境
//	WSADATA wsaData;
//	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != NO_ERROR)
//	{
//		printf("Error at WSAStartup()\n");
//		exit(-1);
//	}
//	printf("Server Running in WONDOWS\n");
//#else
//	printf("Server Running in LINUX\n");
//#endif
//
//	SSL_CTX     *ctx;
//	SSL         *ssl;
//	char szBuffer[1024];
//	int nLen;
//	struct sockaddr_in addr;
//	int nListenFd, nAcceptFd;
//
//	nListenFd = ::socket(AF_INET, SOCK_STREAM, 0);
//	if (nListenFd == -1)
//		return -1;
//
//	struct sockaddr_in sin;
//	sin.sin_family = AF_INET;
//	sin.sin_addr.s_addr = 0;
//	sin.sin_port = htons(SERVER_PORT);
//
//	if (::bind(nListenFd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
//		return -1;
//
//	if (::listen(nListenFd, 5) < 0)
//		return -1;
//
//
//	SSLeay_add_ssl_algorithms();
//	OpenSSL_add_all_algorithms();
//	SSL_load_error_strings();
//	ERR_load_BIO_strings();
//
//	ctx = SSL_CTX_new(SSLv23_method());
//	if (ctx == NULL)
//	{
//		printf("SSL_CTX_new error!\n");
//		return -1;
//	}
//
//	// 是否要求校验对方证书 此处不验证客户端身份所以为： SSL_VERIFY_NONE
//	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
//
//	// 加载CA的证书  
//	if (!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
//	{
//		printf("SSL_CTX_load_verify_locations error!\n");
//		return -1;
//	}
//
//	// 加载自己的证书  
//	if (SSL_CTX_use_certificate_file(ctx, SERVER_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
//	{
//		printf("SSL_CTX_use_certificate_file error!\n");
//		return -1;
//	}
//
//	// 加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机
//	// 消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟
//	// 客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
//	if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
//	{
//		printf("SSL_CTX_use_PrivateKey_file error!\n");
//		return -1;
//	}
//
//	// 判定私钥是否正确  
//	if (!SSL_CTX_check_private_key(ctx))
//	{
//		printf("SSL_CTX_check_private_key error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//
//	memset(&addr, 0, sizeof(addr));
//#ifndef WIN32
//	socklen_t len = sizeof(addr);
//#else
//	int len = sizeof(addr);
//#endif
//	nAcceptFd = accept(nListenFd, (struct sockaddr *)&addr, &len);
//
//	// 将连接付给SSL  
//	ssl = SSL_new(ctx);
//	if (!ssl)
//	{
//		printf("SSL_new error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	SSL_set_fd(ssl, nAcceptFd);
//	SSL_set_accept_state(ssl);
//
//	while (1)
//	{
//		int r = SSL_do_handshake(ssl);
//		if (r == 1)
//		{
//			printf("connect success.!\n");
//			break;
//		}
//	}
//	/*while (1)
//	{
//	int n1 = SSL_accept(ssl);
//	if (n1 == -1)
//	{
//	int icode = -1;
//	int n2 = SSL_get_error(ssl, n1);
//	const char* p1 = SSL_state_string(ssl);
//	int iret = SSL_get_error(ssl, icode);
//	printf("SSL_accept error! code = %d, iret = %d, p1:%s\n", icode, iret, p1);
//	}
//	else
//	{
//	break;
//	}
//	}*/
//
//	while (1)
//	{
//		// 进行操作  
//		memset(szBuffer, 0, sizeof(szBuffer));
//		nLen = SSL_read(ssl, szBuffer, sizeof(szBuffer));
//		if (nLen > 0)
//		{
//			fprintf(stderr, "Get Len %d %s ok\n", nLen, szBuffer);
//			strcat(szBuffer, "\n this is from server========server resend to client");
//			SSL_write(ssl, szBuffer, strlen(szBuffer));
//		}
//	}
//
//
//	// 释放资源  
//	SSL_free(ssl);
//	SSL_CTX_free(ctx);
//	closesocket(nAcceptFd);
//
//	return 0;
//}
//
//int test_ssl_client()
//{
//	static constexpr short SERVER_PORT = 31001;
//	static constexpr const char* CA_CERT_FILE = "ca/ca.crt";
//	static constexpr const char* CLIENT_CERT_FILE = "server/server.crt";
//	static constexpr const char* CLIENT_KEY_FILE = "server/server.key";
//
//#ifdef WIN32
//	//windows初始化网络环境
//	WSADATA wsaData;
//	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != NO_ERROR)
//	{
//		printf("Error at WSAStartup()\n");
//		exit(-1);
//	}
//	printf("Server Running in WONDOWS\n");
//#else
//	printf("Server Running in LINUX\n");
//#endif
//
//	SSL_CTX     *ctx;
//	SSL         *ssl;
//
//	int nFd;
//	int nLen;
//	char szBuffer[1024];
//
//	SSLeay_add_ssl_algorithms();
//	OpenSSL_add_all_algorithms();
//	SSL_load_error_strings();
//	ERR_load_BIO_strings();
//
//	// 使用SSL V3,V2  
//	ctx = SSL_CTX_new(SSLv23_method());
//	if (ctx == NULL)
//	{
//		printf("SSL_CTX_new error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//
//	// 要求校验对方证书，表示需要验证服务器端，若不需要验证则使用  SSL_VERIFY_NONE
//	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
//
//
//	// 加载CA的证书
//	printf("SSL_CTX_load_verify_locations start!\n");
//	if (!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
//	{
//		printf("SSL_CTX_load_verify_locations error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//
//	// 加载自己的证书  
//	if (SSL_CTX_use_certificate_file(ctx, CLIENT_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
//	{
//		printf("SSL_CTX_use_certificate_file error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//
//	//// 加载自己的私钥 加载私钥需要密码，意思是每次链接服务器都需要密码
//	//// 若服务器需要验证客户端的身份，则需要客户端加载私钥，由于此处我们只需要验证服务器身份，故无需加载自己的私钥
//	//printf("SSL_CTX_use_PrivateKey_file start!\n");
//	//if(SSL_CTX_use_PrivateKey_file(ctx, CLIENT_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
//	//{
//	// printf("SSL_CTX_use_PrivateKey_file error!\n");
//	// ERR_print_errors_fp(stderr);
//	// return -1;
//	//}
//
//	//// 判定私钥是否正确  
//	//if(!SSL_CTX_check_private_key(ctx))
//	//{
//	// printf("SSL_CTX_check_private_key error!\n");
//	// ERR_print_errors_fp(stderr);
//	// return -1;
//	//}
//
//	// 创建连接  
//	nFd = ::socket(AF_INET, SOCK_STREAM, 0);
//
//	struct sockaddr_in addr;
//	addr.sin_family = AF_INET;
//	addr.sin_port = htons(SERVER_PORT);
//
//	//链接服务器 
//	if (connect(nFd, (sockaddr *)&addr, sizeof(addr)) < 0)
//	{
//		printf("connect\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//
//	// 将连接付给SSL  
//	ssl = SSL_new(ctx);
//	if (ssl == NULL)
//	{
//		printf("SSL_new error!\n");
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	SSL_set_fd(ssl, nFd);
//	int n1 = SSL_connect(ssl);
//	if (n1 == -1)
//	{
//		int n2 = SSL_get_error(ssl, n1);
//		const char* p1 = SSL_state_string(ssl);
//		cout << p1 << endl;
//	}
//
//	while (1)
//	{
//		// 进行操作  
//		sprintf(szBuffer, "\nclient send to server");
//		SSL_write(ssl, szBuffer, strlen(szBuffer));
//
//		memset(szBuffer, 0, sizeof(szBuffer));
//		nLen = SSL_read(ssl, szBuffer, sizeof(szBuffer));
//		if (nLen > 0)
//		{
//			fprintf(stderr, "Get Len %d %s ok\n", nLen, szBuffer);
//		}
//	}
//
//	SSL_free(ssl);
//	SSL_CTX_free(ctx);
//	closesocket(nFd);
//
//	return 0;
//}

void test_type_trait()
{


}

//void test_log()
//{
//	char a = '1';
//	uint32 b = 199;
//	uint64 c = 299;
//	int32 d = 399;
//	int64 e = 499;
//	float f = 599.1f;
//	double g = 699.2131;
//	char buf[] = { 0,1,2,3 };
//	const char* p = "abcd";
//	std::string str = "qwer";
//
//
//	auto fun_benchmark = [&]()
//	{
//		constexpr int count = 100000;
//		char const * const benchmark = "benchmark";
//
//		uint32 _s = getMSTime();
//
//		for (int i = 0; i < count; ++i)
//		{
//			LOG_INFO << "count " << i;
//		}
//
//		cout << "benchmark cost us:" << getMSTimeDiff(_s, getMSTime()) << endl;
//	};
//
//	constexpr int thread_count = 5;
//	cout << "Thread count:" << thread_count << endl;
//
//	std::vector <std::thread> threads;
//	for (int i = 0; i < thread_count; ++i)
//	{
//		threads.emplace_back(fun_benchmark);
//	}
//	for (int i = 0; i < thread_count; ++i)
//	{
//		threads[i].join();
//	}
//}
//
//MysqlModule mysql_module(0);
//void test_mysql()
//{
//	enum TestTableStatements : uint32
//	{
//		TEST_1,
//		TEST_2,
//		TEST_3,
//		TEST_MAX
//	};
//	class TestTable : public MySQLConnection
//	{
//	public:
//
//		using Statements = TestTableStatements;
//		TestTable(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
//		TestTable(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}
//		~TestTable() {}
//
//		void doPrepareStatements() override
//		{
//			if (!isReconnecting_)
//				vecStmts_.resize(TestTableStatements::TEST_MAX);
//
//			prepareStatement(TestTableStatements::TEST_1, "insert into person(age, name) value(?, ?)", CONNECTION_ASYNC);
//			prepareStatement(TestTableStatements::TEST_2, "update person set age = ? where name = ?", CONNECTION_ASYNC);
//			prepareStatement(TestTableStatements::TEST_3, "select * from person", CONNECTION_ASYNC);
//		}
//	};
//
//	QueryCallbackProcessor queryProcessor_;
//	DatabaseWorkerPool<TestTable> dbTestTable_;
//	DatabaseLoader dbLoader_;
//	dbLoader_.addDatabase(dbTestTable_, "127.0.0.1;3306;root;8292340;test_table", 5);
//	dbLoader_.load();
//	
//	std::atomic<int> op{ -1 };
//	std::thread thr([&]()
//	{
//		while (1)
//		{
//			int input;
//			std::cin >> input;
//			op = input;
//		}
//	});
//
//	while (1)
//	{
//		queryProcessor_.processReadyQueries();
//
//		if (op == 1)
//		{
//			auto lamda = [](PreparedQueryResult result)
//			{
//				if (result)
//				{
//					do
//					{
//						Field* fields = result->fetch();
//					} while (result->nextRow());
//				}
//			};
//			for (int i = 0; i < 5; ++i)
//			{
//				PreparedStatement* stmt = dbTestTable_.getPreparedStatement(TEST_1);
//				stmt->setInt16(0, i);
//				stmt->setString(1, "abcd");
//				queryProcessor_.addQuery(dbTestTable_.asyncExecuteWithResult(stmt).withPreparedCallback(std::move(lamda)));
//			}
//		}
//		else if (op == 2)
//		{
//			PreparedStatement* stmt = dbTestTable_.getPreparedStatement(TEST_3);
//			QueryResult result = dbTestTable_.syncExecuteWithResult("select * from person");
//			if (result)
//			{
//				do
//				{
//					Field* fields = result->fetch();
//					cout << "age: " << fields[0].getInt32() << endl;
//					cout << "name: " << fields[1].getString() << endl;
//				} while (result->nextRow());
//			}
//		}
//		else if (op == 3)
//		{
//			auto lamda = [](QueryResult result)
//			{
//				if (result)
//				{
//					do
//					{
//						Field* fields = result->fetch();
//						cout << "age: " << fields[0].getInt32() << endl;
//						cout << "name: " << fields[1].getString() << endl;
//					} while (result->nextRow());
//				}
//			};
//			PreparedStatement* stmt = dbTestTable_.getPreparedStatement(TEST_3);
//			queryProcessor_.addQuery(dbTestTable_.asyncExecuteWithResult(stmt).withCallback(std::move(lamda)));
//		}
//
//		op = -1;
//	}
//
//	thr.join();
//}


//class ExampleBehaviour : public CoroBehaviour
//{
//private:
//	std::stringstream Stream;
//
//public:
//	//Enumerator TaskA()
//	//{
//	//	return [=](CoroPush& yield_return)
//	//	{
//	//		Stream << "A1 ";
//	//		yield_return(nullptr);
//
//	//		Stream << "A2 ";
//	//	};
//	//}
//
//	Enumerator TaskB(float Param)
//	{
//		return [=](CoroPush& yield_return)
//		{
//			cout << "TaskB1 " << endl;
//			yield_return(new WaitForSeconds(Param));
//		};
//	}
//
//	//Enumerator TaskC(float Param)
//	//{
//	//	return [=](CoroPush& yield_return)
//	//	{
//	//		Stream << "C1 ";
//	//		yield_return(StartCoroutine(TaskA()));
//
//	//		Stream << "C2 ";
//	//		yield_return(StartCoroutine(TaskB(Param)));
//
//	//		Stream << "C3 ";
//	//	};
//	//}
//
//	//Enumerator TaskD(int Param)
//	//{
//	//	return [=](CoroPush& yield_return)
//	//	{
//	//		for (int i = 1; i < Param; i++)
//	//		{
//	//			Stream << "D" << i << " ";
//	//			yield_return(new WaitForSeconds(0.5f));
//	//		}
//	//		Stream << "D" << Param << " ";
//	//	};
//	//}
//
//	void Test()
//	{
//		/*while (1)
//		{
//			std::this_thread::sleep_for(std::chrono::seconds(1));
//			cout << 11 << endl;
//
//			foo.StartCoroutine(foo.TaskB(5.0f));
//
//			while (HasCoroutines())
//			{
//				ResumeCoroutines();
//
//				auto str = Stream.str();
//				if (str.size() > 0)
//				{
//					std::cout << str << std::endl;
//					Stream.str("");
//				}
//			}
//		}*/
//	}
//};

void test_coroutine()
{
	/*auto foo = [](boost::coroutines2::coroutine<void>::push_type& sink) {
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "a ";
	};

	while (1)
	{
	boost::coroutines2::coroutine<void>::pull_type source(foo);
	while (source)
	{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	cout << "while" << endl;
	}
	}*/

	//ExampleBehaviour foo;
	////foo.StartCoroutine(foo.TaskA());
	////foo.StartCoroutine(foo.TaskB(5.0f));
	////foo.StartCoroutine(foo.TaskC(2.0f));
	////foo.StartCoroutine(foo.TaskD(10));
	//foo.Test();

	//while (1)
	//{
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
	//	cout << 11 << endl;

	//	foo.StartCoroutine(foo.TaskB(5.0f));

	//	while (foo.HasCoroutines())
	//	{
	//		foo.ResumeCoroutines();
	//	}
	//}
}


CoroutineManager cm;
int cal_num = 0;

void cal_count()
{
	cm.yieldCo();
	for (int i = 0; i < 1000000; ++i)
	{
		if (cal_num >= 5)
		{
			cout << "cal_count finish" << endl;
			break;
		}
	}
}
void run(void*)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	cout << "run" << endl;
	cal_num++;

	cal_count();
}
int main()
{
#ifdef _WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#endif

	zqlog::LogSingle::getInstance().init(zqlog::GuaranteedLogger(), "log/", "test_log");
	//LibManager::get_instance().launch(argc, argv);

	// test_log();
	//test_mysql();
	//test_type_trait();
	//test_http_client();
	//test_ssl_server();

	//test_coroutine();

	cm.init(run);
	while (1)
	{
		cm.scheduleJob();
	}
	
	std::getchar();
	return 0;
}





