#include "libmanager.h"
#include "dependencies/RapidXML/rapidxml.hpp"
#include "dependencies/RapidXML/rapidxml_iterators.hpp"
#include "dependencies/RapidXML/rapidxml_print.hpp"
#include "dependencies/RapidXML/rapidxml_utils.hpp"
#include "interface_header/base/ILib.h"
#include "self_register_factory.hpp"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/ILogModule.h"
#include "interface_header/base/IConsoleCommandModule.h"
#include "baselib/base_code/util.h"
#include "baselib/message/config_define.hpp"
#include "signal_handler.h"
#include <future>

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <windows.h>
#include <dbghelp.h>
#else
#include <sys/file.h>	// flock()
#include <signal.h>		// kill()
#endif



#ifndef ZQ_DYNAMIC_PLUGIN

#include "baselib/kernel/kernel.h"
#include "baselib/network/network.h"
#include "baselib/log_lib/log_lib.h"
#include "baselib/message/message_lib.h"
#include "baselib/db_redis/redis_lib.h"
#include "baselib/http_lib/http_lib.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ws2_32.lib")	 
#pragma comment(lib, "libprotobuf_d.lib")											  
#pragma comment(lib, "base_code_d.lib")
#pragma comment(lib, "kernel_d.lib")
#pragma comment(lib, "core_d.lib")
#pragma comment(lib, "network_d.lib")
#pragma comment(lib, "log_lib_d.lib")
#pragma comment(lib, "message_d.lib")
#pragma comment(lib, "db_redis_d.lib")
#pragma comment(lib, "db_mysql_d.lib")
#pragma comment(lib, "http_lib_d.lib")
#endif

#endif

using namespace zq;



constexpr const char* ROOT_DIR_NAME = "../datacfg/";
constexpr const char* XML_DIR_NAME = "xml/";
constexpr const char* LAUNCH_NAME = "launch.xml";

constexpr int APPCMD_NOTHING_TODO = 0;          // 啥也不干
constexpr int APPCMD_STOP_SERVICE = 1;          // 停止命令
constexpr int APPCMD_RELOAD_CONFIG = 2;         // 重读配置命令
constexpr int APPCMD_QUIT_SERVICE = 3;          // 立刻跳出循环


static std::string s_lock_file_name;
static std::string s_pid_file_name;


int LibManager::cmdState_ = APPCMD_NOTHING_TODO;
std::shared_ptr<std::thread> LibManager::s_thread_ = nullptr;

void coroutineExecute(void* arg)
{
	LibManager::get_instance().run();
}

// wind下程序dump
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
static void createDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
	//Dump
	HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

static long applicationCrashHandler(EXCEPTION_POINTERS* pException)
{
	time_t t = time(0);
	char szDmupName[MAX_PATH];
	tm* ptm = localtime(&t);

	sprintf_s(szDmupName, "%04d_%02d_%02d_%02d_%02d_%02d.dmp", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	createDumpFile(szDmupName, pException);

	FatalAppExit(-1, szDmupName);

	return EXCEPTION_EXECUTE_HANDLER;
}

static void closeXButton()
{
	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}
}

static bool applicationCtrlHandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	{
		LibManager::setCmd(APPCMD_STOP_SERVICE);
	}
	return true;
	default:
		return false;
	}
}

#else

int checkLock(const std::string& name)
{
	int iLockFD = open(name.c_str(), O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (iLockFD < 0)
	{
		return -1;
	}

	if (flock(iLockFD, LOCK_EX | LOCK_NB) < 0)
	{
		return -2;
	}

	return 0;
}

void writePidFile(const std::string& name)
{
	util::createPIDFile(name);
}

void cleanPidFile()
{
	unlink(s_pid_file_name.c_str());
}

int readPidFile(const std::string& name)
{
	FILE* fp;

	fp = fopen(name.c_str(), "r");
	if (NULL == fp)
	{
		printf("Failed to open pid file: %s\n", name.c_str());
		return -1;
	}

	char szPid[100];
	fread(szPid, sizeof(szPid), 1, fp);
	int iPid = atoi(szPid);

	fclose(fp);

	printf("ReadPid: %d\n", iPid);

	return iPid;
}

void daemonLaunch()
{
	// int daemon(int nochdir, int noclose);  
	// nochdir为零时,当前目录变为根目录,否则不变
	// noclose为零时，标准输入,标准输出和错误输出重导向为/dev/null,也就是不输出任何信息,否则照样输出
	// 返回值:
	// deamon()调用了fork(),如果fork成功,那么父进程就调用_exit(2)退出
	// 所以看到的错误信息全部是子进程产生的,如果成功函数返回0,否则返回-1并设置errno
	daemon(1, 0);
	SignalHandler::ignoreSignalSet();
}

void registerSignalHandler(SignalFunT fun)
{
	// usr1为重新加载配置
	SignalHandler::setHandlerUsr1(fun, APPCMD_RELOAD_CONFIG);

	// usr2为停止服务器
	SignalHandler::setHandlerUsr2(fun, APPCMD_STOP_SERVICE);
}
#endif

void LibManager::threadFunc()
{
	while (cmdState_ != APPCMD_STOP_SERVICE)
	{
		sleep(50);

		std::string s;
		std::cin >> s;
		if (s.find("exit") != std::string::npos)
		{
			cmdState_ = APPCMD_STOP_SERVICE;
		}
		else if (s.find("reload=") != std::string::npos)
		{
			std::string name = s.substr(s.find_last_of('=') + 1, s.length() - s.find_last_of('=') - 1);
			LibManager::get_instance().reload(name);
		}
		else if (s.find("account=") != std::string::npos)
		{
			LibManager::get_instance().processConsoleCmd(s);
		}
	}
}

void LibManager::shutdownCliThread(std::thread* cliThread)
{
	if (cliThread != nullptr)
	{
		cliThread->join();
		delete cliThread;
	}
}

void LibManager::setCmd(int cmd)
{
	LibManager::cmdState_ = cmd;
}

LibManager::LibManager() : ILibManager(),
	serverId_(0),
	serverType_(0),
	serverName_(""),
	initTime_(0),
	nowTime_(0),
	getFileContentFunctor_ (nullptr),
	cfgRootDir_(ROOT_DIR_NAME),
	cfgXmlDir_(cfgRootDir_ + XML_DIR_NAME),
	launchConfigName_(LAUNCH_NAME)
{
}

LibManager::~LibManager()
{

}

bool LibManager::launch(std::initializer_list<ILib*>& libs, SERVER_TYPES server_type, int argc, char* argv[])
{
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	WSADATA wsaData;
	if (0 !=WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		ASSERT(false);
		return false;
	}

	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)applicationCrashHandler);
#endif
	  
	serverType_ = server_type;
	serverName_ = serverType2Name(server_type);

	parseParameter(argc, argv);

	// 日志
	std::string strServerName = serverName_ + std::to_string(serverId_);
	zqlog::LogSingle::getInstance().init(zqlog::GuaranteedLogger(), "log/", strServerName);

	// 打印相关信息
	printInfo();

	// 注册相关库
	registerCommLib();

	for (const auto& ref : libs)
	{
		registerLib(ref);
	}

	init();
	initEnd();
	checkConfig();

	return true;
}

bool LibManager::loop()
{
	uint64_t num = 0;
	while (1)
	{
		if (LibManager::cmdState_ == APPCMD_STOP_SERVICE)
		{
			ILogModule* logModule = ILibManager::findModule<ILogModule>();
			logModule->info("Receive Command: APPCMD_STOP_SERVICE\n");
			logModule->info("==============================================!\n");
			break;
		}
		else if (LibManager::cmdState_ == APPCMD_RELOAD_CONFIG)
		{
			ILogModule* logModule = ILibManager::findModule<ILogModule>();
			logModule->info("Receive Command: APPCMD_RELOAD_CONFIG\n");
			logModule->info("==============================================!\n");
			cmdState_ = APPCMD_NOTHING_TODO;
		}

		tryRun();

		sleep(1);

		num++;
	}

	std::cout << "server exit...." << std::endl;

	destruct();

	// 让用户能看到窗口信息
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	sleep(2000);
#endif

	return true;
}

void LibManager::stop()
{
	cmdState_ = APPCMD_STOP_SERVICE;
}

void LibManager::destruct()
{
	beforeShut();
	shut();
	finalize();
	s_thread_->join();
}

int LibManager::parseParameter(int argc, char* argv[])
{
	std::string strArgvList;
	for (int i = 0; i < argc; i++)
	{
		strArgvList += " ";
		strArgvList += argv[i];
	}

#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
	bool bDeamon = false;
	if (strArgvList.find("-d") != std::string::npos)
	{
		bDeamon = true;
	}
#endif

	std::string str_cmd = "";
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg.find("cfg=") != std::string::npos)
		{
			arg.erase(0, 4);
			launchConfigName_ = arg;
		}
		else if (arg.find("ID=") != std::string::npos)
		{
			arg.erase(0, 3);
			strto(arg, serverId_);
		}
		else if (arg.find("cmd=") != std::string::npos)
		{
			arg.erase(0, 4);
			str_cmd = arg;
		}
	}

	std::string strServerName = serverName_ + "-" + std::to_string(serverId_);

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN

	SetConsoleTitle(strServerName.c_str());
	createBackThread();

#elif ZQ_PLATFORM == ZQ_PLATFORM_UNIX

	s_lock_file_name = strServerName + ".lock";
	s_pid_file_name = strServerName + ".pid";
	if (str_cmd == "stop")
	{
		int iPid = readPidFile(s_pid_file_name.c_str());
		if (iPid > 0)
		{
			kill(iPid, SIGUSR2);
		}

		::exit(0);
	}
	else if (str_cmd == "reload")
	{
		int iPid = readPidFile(s_pid_file_name.c_str());
		if (iPid > 0)
		{
			kill(iPid, SIGUSR1);
		}

		::exit(0);
	}

	// 检测是否已经同样的服务已经启动了
	int ret = checkLock(s_lock_file_name);
	if (ret != 0)
	{
		std::cout << "checklock faild, maybe sameserver running!, ret: " << ret << std::endl;
		::exit(1);
	}

	// deamon方式启动
	if (bDeamon)
	{
		daemonLaunch();
	}

	// 设置USR信号处理, 用于停止和重载配置
	registerSignalHandler(LibManager::setCmd);

	// 写入PID文件
	writePidFile(s_pid_file_name);

	//退出时候删除pid文件
	atexit(cleanPidFile);

#endif

	return 0;
}

bool LibManager::printInfo()
{
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

	return true;
}

bool LibManager::createBackThread()
{
	s_thread_.reset(new std::thread(&LibManager::threadFunc), &LibManager::shutdownCliThread);
	return true;
}

bool LibManager::init()
{
	// 加载配置
	//loadConfig();

	// 用于动态库
	for (auto it = veclibName_.begin(); it != veclibName_.end(); ++it)
	{
#ifdef ZQ_DYNAMIC_PLUGIN
		loadDynLibrary(*it);
#endif
	}

	for (auto it = libInstanceMap_.begin(); it != libInstanceMap_.end(); ++it)
	{
		it->second->init();
	}

	return true;
}

bool LibManager::loadConfig()
{
	/*std::string strContent;
	std::string strFilePath = launchConfigName_;
	getFileContent(strFilePath, strContent);

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

	rapidxml::xml_node<>* pRoot = xDoc.first_node();
	rapidxml::xml_node<>* pAppNameNode = pRoot->first_node(serverName_.c_str());
	if (!pAppNameNode)
	{
		ASSERT(0, "There are no App ID", __FILE__, __FUNCTION__);
		return false;
	}

	for (rapidxml::xml_node<>* pPluginNode = pAppNameNode->first_node("Lib"); pPluginNode; pPluginNode = pPluginNode->next_sibling("Lib"))
	{
		const char* lib_name = pPluginNode->first_attribute("Name")->value();

		libNameMap_.insert(LibNameMap::value_type(lib_name, true));
	}*/

    return true;
}

void LibManager::registerLib(ILib* ilib)
{
    std::string lib_name = ilib->getLibName();
    if (!findLib(lib_name))
	{
		libInstanceMap_.insert(std::make_pair(lib_name, ilib));
		ilib->install();
    }
	else
	{						    
		ASSERT(false);							    
	}											  
}												   

void LibManager::unRegisterLib(ILib* lib)
{
    auto it = libInstanceMap_.find(lib->getLibName());
    if (it != libInstanceMap_.end())
    {
        it->second->uninstall();
        delete it->second;
        it->second = NULL;
        libInstanceMap_.erase(it);
    }
}

bool LibManager::reLoadDynLib(const std::string & lib_name)
{
	auto itInstance = libInstanceMap_.find(lib_name);
	if (itInstance == libInstanceMap_.end())
	{
		return false;
	}

	//ILib* pLib = itInstance->second;
	//IModule* pModule = pLib->first();
	//while (pModule)
	//{
	//	pModule->beforeShut();
	//	pModule->shut();
	//	pModule->finalize();

	//	pModule = pLib->next();
	//}

	auto it = libMap_.find(lib_name);
	if (it != libMap_.end())
	{
		DynLib* pLib = it->second;
		DLL_STOP_FUNC pFunc = (DLL_STOP_FUNC)pLib->getSymbol("dllStop");

		if (pFunc)
		{
			pFunc(this);
		}

		pLib->unLoad();

		delete pLib;
		pLib = NULL;
		libMap_.erase(it);
	}

	DynLib* pLib = new DynLib(lib_name);
	bool bLoad = pLib->load();
	if (bLoad)
	{
		libMap_.insert(std::make_pair(lib_name, pLib));

		DLL_START_FUNC pFunc = (DLL_START_FUNC)pLib->getSymbol("dllStart");
		if (!pFunc)
		{
			std::cout << "Reload find function dllStart Failed in [" << pLib->getName() << "]" << std::endl;
			assert(0);
			return false;
		}

		pFunc(this);
	}
	else
	{
#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
		char* error = dlerror();
		if (error)
		{
			std::cout << stderr << " Reload shared lib[" << pLib->getName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
			std::cout << "Reload [" << pLib->getName() << "] failed" << std::endl;
			assert(0);
			return false;
		}
#elif ZQ_PLATFORM == ZQ_PLATFORM_WIN
		std::cout << stderr << " Reload DLL[" << pLib->getName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
		std::cout << "Reload [" << pLib->getName() << "] failed" << std::endl;
		assert(0);
		return false;
#endif 
	}

	auto itReloadInstance = libInstanceMap_.begin();
	for (; itReloadInstance != libInstanceMap_.end(); itReloadInstance++)
	{
		if (lib_name != itReloadInstance->first)
		{
			itReloadInstance->second->onReload();
		}
	}
	return true;
}

ILib* LibManager::findLib(const std::string& lib_name)
{
    auto it = libInstanceMap_.find(lib_name);
    if (it != libInstanceMap_.end())
    {
        return it->second;
    }

    return nullptr;
}

bool LibManager::run()
{
    nowTime_ = time(NULL);

    bool bRet = true;

    for (auto it = libInstanceMap_.begin(); it != libInstanceMap_.end(); ++it)
    {
        bool tembRet = it->second->run();
        bRet = bRet && tembRet;
    }

    return bRet;
}

void LibManager::tryRun()
{
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	__try
	{
#endif
		LibManager::get_instance().run();
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	}
	__except (applicationCrashHandler(GetExceptionInformation()))
	{

	}
#endif
}

void LibManager::setGetFileContentFunctor(GET_FILECONTENT_FUNCTOR fun)
{
	getFileContentFunctor_ = fun;
}

bool LibManager::getFileContent(const std::string &strFileName, std::string &strContent)
{
	if (getFileContentFunctor_)
	{
		return getFileContentFunctor_(strFileName, strContent);
	}

	FILE *fp = fopen(strFileName.c_str(), "rb");
	if (!fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	const long filelength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	strContent.resize(filelength);
	fread((void*)strContent.data(), filelength, 1, fp);
	fclose(fp);

	return true;
}

void LibManager::addModule(const std::string& strModuleName, IModule* pModule)
{
    if (!_findModule(strModuleName))
    {
        moduleInstanceMap_.insert(ModuleInstanceMap::value_type(strModuleName, pModule));
    }
}

void LibManager::removeModule(const std::string& strModuleName)
{
    ModuleInstanceMap::iterator it = moduleInstanceMap_.find(strModuleName);
    if (it != moduleInstanceMap_.end())
    {
        moduleInstanceMap_.erase(it);
    }
}

IModule* LibManager::_findModule(const std::string& strModuleName)
{
	auto it = moduleInstanceMap_.find(strModuleName);
	if (it != moduleInstanceMap_.end())
	{
		return it->second;
	}

    return nullptr;
}

bool LibManager::initEnd()
{
    auto itAfterInstance = libInstanceMap_.begin();
    for (; itAfterInstance != libInstanceMap_.end(); itAfterInstance++)
    {
        itAfterInstance->second->initEnd();
    }

    return true;
}

bool LibManager::checkConfig()
{
    auto itCheckInstance = libInstanceMap_.begin();
    for (; itCheckInstance != libInstanceMap_.end(); itCheckInstance++)
    {
        itCheckInstance->second->checkConfig();
    }

    return true;
}

bool LibManager::beforeShut()
{
    auto itBeforeInstance = libInstanceMap_.begin();
    for (; itBeforeInstance != libInstanceMap_.end(); itBeforeInstance++)
    {
        itBeforeInstance->second->beforeShut();
    }

    return true;
}

bool LibManager::shut()
{
	auto itInstance = libInstanceMap_.begin();
    for (; itInstance != libInstanceMap_.end(); ++itInstance)
    {
        itInstance->second->shut();
    }

    return true;
}

bool LibManager::finalize()
{
	for (auto it = libInstanceMap_.begin(); it != libInstanceMap_.end(); it++)
	{
		it->second->finalize();
		delete it->second;
	}

	for (auto it = veclibName_.begin(); it != veclibName_.end(); it++)
	{
#ifdef ZQ_DYNAMIC_PLUGIN
		unLoadDynLibrary(*it);
#endif
	}

	libInstanceMap_.clear();
	veclibName_.clear();

	return true;
}

bool LibManager::loadDynLibrary(const std::string& lib_name)
{
    auto it = libMap_.find(lib_name);
    if (it == libMap_.end())
    {
        DynLib* pLib = new DynLib(lib_name);
        bool bLoad = pLib->load();

        if (bLoad)
        {
            libMap_.insert(std::make_pair(lib_name, pLib));

            DLL_START_FUNC pFunc = (DLL_START_FUNC)pLib->getSymbol("dllStart");
            if (!pFunc)
            {
                std::cout << "find function dllStart Failed in [" << pLib->getName() << "]" << std::endl;
                assert(0);
                return false;
            }

            pFunc(this);

            return true;
        }
        else
        {
#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX || ZQ_PLATFORM == ZQ_PLATFORM_APPLE
            char* error = dlerror();
            if (error)
            {
                std::cout << stderr << " load shared lib[" << pLib->getName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
                std::cout << "load [" << pLib->getName() << "] failed" << std::endl;
                assert(0);
                return false;
            }
#elif ZQ_PLATFORM == ZQ_PLATFORM_WIN
            std::cout << stderr << " load DLL[" << pLib->getName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
            std::cout << "load [" << pLib->getName() << "] failed" << std::endl;
            assert(0);
            return false;
#endif 
        }
    }

    return false;
}

bool LibManager::unLoadDynLibrary(const std::string& lib_name)
{
    auto it = libMap_.find(lib_name);
    if (it != libMap_.end())
    {
        DynLib* pLib = it->second;
        DLL_STOP_FUNC pFunc = (DLL_STOP_FUNC)pLib->getSymbol("dllStop");

        if (pFunc)
        {
            pFunc(this);
        }

        pLib->unLoad();

        delete pLib;
        pLib = nullptr;
        libMap_.erase(it);

        return true;
    }

    return false;
}

bool LibManager::reload(const std::string& cfg_name)
{
	/*IElementModule* elementModule = ILibManager::findModule<IElementModule>();
	IClassModule* classModule = ILibManager::findModule<IClassModule>();

	{
		std::shared_ptr<IClass> logic_class = classModule->getElement(config::Redis::this_name());
		if (logic_class)
		{
			const std::vector<std::string>& strIdList = logic_class->getIDList();
			for (size_t i = 0; i < strIdList.size(); ++i)
			{
				const std::string& strId = strIdList[i];

				const int nPort = elementModule->getPropertyInt(strId, config::Redis::port());
				const std::string& strIP = elementModule->getPropertyString(strId, config::Redis::ip());
				const std::string& strAuth = elementModule->getPropertyString(strId, config::Redis::auth());

				std::cout << "strId: " << strId << std::endl;
				std::cout << "nPort: " << nPort << std::endl;
				std::cout << "strIP: " << strIP << std::endl;
				std::cout << "strAuth: " << strAuth << std::endl;
			}
		}
	}

	std::cout << "reload file: " << cfg_name << std::endl;
	if (elementModule->reload(cfg_name))
	{
		std::cout << "reload file success!" << std::endl;
	}
	else
	{
		std::cout << "reload file failed!" << std::endl;
		return false;
	}	*/

	return true;
}

void LibManager::registerCommLib()
{
	CREATE_LIB(this, KernelLib);
	CREATE_LIB(this, LogLib);
	CREATE_LIB(this, NetworkLib);
	CREATE_LIB(this, MessageLib);
	CREATE_LIB(this, RedisLib);
	CREATE_LIB(this, HttpLib);
}

void LibManager::processConsoleCmd(const std::string& cmd)
{
	ILibManager::findModule<IConsoleCommandModule>()->process(cmd);
}