#pragma once 


#include "lib/interface_header/platform.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN

#define DYNLIB_HANDLE hInstance
#define DYNLIB_LOAD( a ) LoadLibraryExA( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#define DYNLIB_UNLOAD( a ) FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif ZQ_PLATFORM == ZQ_PLATFORM_UNIX
#include <dlfcn.h>
#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#define DYNLIB_UNLOAD( a ) dlclose( a )

#elif ZQ_PLATFORM == ZQ_PLATFORM_APPLE
#include <dlfcn.h>
#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD( a ) dlopen( a, RTLD_LOCAL|RTLD_LAZY)
#define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#define DYNLIB_UNLOAD( a ) dlclose( a )

#endif

namespace zq {

class DynLib
{

public:

	DynLib(const std::string& strName)
	{
		mbMain = false;
		mstrName = strName;
#ifdef ZQ_DEBUG_MODE
		mstrName.append("_d");
#endif

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
		mstrName.append(".dll");
#elif ZQ_PLATFORM == ZQ_PLATFORM_UNIX
		mstrName.append(".so");
#elif ZQ_PLATFORM == ZQ_PLATFORM_APPLE 
		mstrName.append(".so");
#endif

		printf("LoadPlugin:%s\n", mstrName.c_str());
	}

	~DynLib()
	{

	}

	bool load();

	bool unLoad();

	const std::string& getName(void) const
	{
		return mstrName;
	}

	const bool GetMain(void) const
	{
		return mbMain;
	}

	void* getSymbol(const char* szProcName);

protected:

	std::string mstrName;
	bool mbMain;

	DYNLIB_HANDLE mInst;
};

}
