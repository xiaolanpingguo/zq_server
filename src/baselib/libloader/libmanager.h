#pragma once


#include <map>
#include <time.h>
#include <unordered_map>
#include "coroutine_manager.h"
#include "interface_header/base/IModule.h"
#include "interface_header/base/ILibManager.h"
#include "interface_header/base/singleton.hpp"



namespace zq {
class DynLib;

class LibManager : public ILibManager, public Singleton<LibManager>
{
public:
	LibManager();
	virtual ~LibManager();

	template<SERVER_TYPES ServerType, typename... Args>
	bool launch(int argc, char* argv[])
	{
		std::initializer_list<ILib*> arr{ ((void)0, new Args(this))... };
		return launch(arr, ServerType, argc, argv);
	}

	bool launch(std::initializer_list<ILib*>& lib, SERVER_TYPES server_type, int argc, char* argv[]);
	bool loop();
	void stop() override;
	void destruct();

	bool init() override;
	bool initEnd() override;
	bool checkConfig() override;
	bool beforeShut() override;
	bool shut() override;
	bool finalize() override;
	bool run() override;
	void tryRun();

	// lib相关
	void registerLib(ILib* ilib) override;
	void unRegisterLib(ILib* ilib) override;
	ILib* findLib(const std::string& lib_name) override;

	// dynlib相关
	void registerDynLib(ILib* ilib, DynLib* dynLib) override;
	void unRegisterDynLib(DynLib* dynLib) override;
	DynLib* findDynLib(const std::string& lib_name) override;

	// 模块相关
	void addModule(const std::string& strModuleName, IModule* pModule) override;
	void removeModule(const std::string& strModuleName) override;

	int getServerType() override { return serverType_; }
	int getServerID() override { return serverId_; }

	uint64 getInitTime() const override { return initTime_; }
	uint64 getNowTime() const override { return nowTime_; }

	const std::string& getServerName() const override { return serverName_; }

	const std::string& getCfgRootDir() const override { return cfgRootDir_; }
	const std::string& getCfgXmlDir() const override { return cfgXmlDir_; }
	const std::string& getCfgCsvDir() const override { return cfgCsvDir_; }

	bool getFileContent(const std::string &strFileName, std::string &strContent) override;

	// 用于信号
	static void setCmd(int cmd);

protected:

	// 解析命令参数
	int parseParameter(int argc, char* argv[]);

	// 打印信息
	bool printInfo();

	// 创建后台线程
	bool createBackThread();

	// 重载配置
	bool reload(const std::string& cfg_name);

	// 加载公共lib
	void registerCommLib();

	// 加载DLL
	void registerDll();

	// 处理后台命令
	void processConsoleCmd(const std::string& cmd);

	// corotine
	//void go(CoroutineFun&& fun);

	IModule* _findModule(const std::string& strModuleName);

protected:

	// 用于动态库
	bool reLoadDynLib(const std::string& lib_name);
	bool loadDynLibrary(const std::string& lib_name);
	bool unLoadDynLibrary(const std::string& lib_name);

protected:

	// 后台线程
	static void threadFunc();
	static void shutdownCliThread(std::thread* cliThread);

	static int cmdState_;

	static std::shared_ptr<std::thread> s_thread_;

private:
	int serverId_;
	int serverType_;
	uint64 nowTime_;
	uint64 initTime_;
	std::string serverName_;
	std::string launchConfigName_;

	std::string cfgRootDir_;
	std::string cfgXmlDir_;
	std::string cfgCsvDir_;

	//CoroutineMgr coroutineMgr_;

	std::vector<std::string> staticLibVec_;

	using VecLibName = std::vector<std::string>;
	using DynLibMap = std::map<std::string, DynLib*>;
	using LibInstanceMap = std::map<std::string, ILib*>;
	using ModuleInstanceMap = std::map<std::string, IModule*>;
	using DLL_START_FUNC = void(*)(ILibManager* p,DynLib* dynLib);
	using DLL_STOP_FUNC = void(*)(ILibManager* p,std::string dynName);

	VecLibName veclibName_;
	DynLibMap dllLibMap_;
	LibInstanceMap libInstanceMap_;
	ModuleInstanceMap moduleInstanceMap_;
};

}