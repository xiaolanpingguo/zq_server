#pragma once


#include <functional>
#include "platform.h"


namespace zq {
class DynLib;


enum SERVER_TYPES : int
{
	ST_SERVER_NONE = 0,
	ST_MASTER_SERVER = 1,
	ST_LOGIN_SERVER = 2,
	ST_WORLD_SERVER = 3,
	ST_GAME_SERVER = 4,
	ST_GATE_SERVER = 5,
	ST_DB_SERVER = 6,
	ST_LOG_SERVER = 7,
	ST_MISC_SERVER = 8,
	ST_TEST_SERVER = 9,
	ST_SERVER_MAX
};

static const int ALL_SERVER_TYPE[] =
{
	ST_MASTER_SERVER,ST_LOGIN_SERVER,ST_WORLD_SERVER,
	ST_GAME_SERVER,ST_GATE_SERVER, ST_DB_SERVER,
	ST_LOG_SERVER, ST_MISC_SERVER,ST_TEST_SERVER
};
static const char* ALL_SERVER_NAME[] =
{
	"server_null", "master_server", "login_server",
	"world_server",	"game_server", "gate_server",
	"db_server","log_server","misc_server","test_server",
	"server_max" ,
};

inline int serverName2Type(const std::string& name)
{
	for (int i = ST_MASTER_SERVER; i < ST_SERVER_MAX; ++i)
	{
		if (strcmp(ALL_SERVER_NAME[i], name.c_str()) == 0)
		{
			return i;
		}
	}

	return ST_SERVER_NONE;
}
inline const char* serverType2Name(int type)
{
	if (type < 0 || type >= ST_SERVER_MAX)
	{
		return ALL_SERVER_NAME[ST_SERVER_NONE];
	}

	return ALL_SERVER_NAME[type];
}


class ILib;
class IModule;

template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
	static int AnyFunction(BaseType* base)
	{
		return 1;
	}

	static  char AnyFunction(void* t2)
	{
		return 0;
	}

	enum
	{
		Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
	};
};

#define FIND_MODULE(classBaseName, className)  \
	assert((TIsDerived<classBaseName, IModule>::Result));



class ILibManager
{
public:
	ILibManager()
	{

	}

	virtual ~ILibManager(){}

	virtual bool init()
	{

		return true;
	}

	virtual bool initEnd()
	{
		return true;
	}

	virtual bool checkConfig()
	{
		return true;
	}

	virtual bool run()
	{
		return true;
	}

	virtual bool beforeShut()
	{
		return true;
	}

	virtual bool shut()
	{
		return true;
	}

	virtual bool finalize()
	{
		return true;
	}

	template <typename T>
	T* findModule()
	{
		IModule* pLogicModule = _findModule(typeid(T).name());
		if (pLogicModule)
		{
			if (!TIsDerived<T, IModule>::Result)
			{
				return NULL;
			}
			//TODO OSX上dynamic_cast返回了NULL
#if ZQ_PLATFORM == ZQ_PLATFORM_APPLE
			T* pT = (T*)pLogicModule;
#else
			T* pT = dynamic_cast<T*>(pLogicModule);
#endif
			assert(NULL != pT);

			return pT;
		}
		assert(NULL);
		return NULL;
	}

	virtual void stop() = 0;

	virtual void registerLib(ILib* ilib) = 0;

	virtual void unRegisterLib(ILib* ilib) = 0;

	virtual ILib* findLib(const std::string& lib_name) = 0;

	virtual void addModule(const std::string& strModuleName, IModule* pModule) = 0;

	virtual void removeModule(const std::string& strModuleName) = 0;

	virtual IModule* _findModule(const std::string& strModuleName) = 0;

	virtual int getServerType() = 0;
	virtual int getServerID() = 0;
	virtual const std::string& getServerName() const = 0;

	virtual const std::string& getCfgRootDir() const = 0;
	virtual const std::string& getCfgXmlDir() const = 0;
	virtual const std::string& getCfgCsvDir() const = 0;

	virtual uint64 getInitTime() const = 0;
	virtual uint64 getNowTime() const = 0;

	virtual bool getFileContent(const std::string &strFileName, std::string &strContent) = 0;

	virtual bool reLoadDynLib(const std::string& lib_name) = 0;
	virtual bool loadDynLibrary(const std::string& lib_name) = 0;
	virtual bool unLoadDynLibrary(const std::string& lib_name) = 0;

	virtual void registerDynLib(ILib* ilib,DynLib* dynLib) = 0;
	virtual void unRegisterDynLib(DynLib* dynLib) = 0;
	virtual DynLib* findDynLib(const std::string& lib_name) = 0;
};

}

