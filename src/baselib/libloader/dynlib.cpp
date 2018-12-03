#include "dynlib.h"
#include "interface_header/base/ILib.h"
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <windows.h>
#endif
using namespace zq;


bool DynLib::load()
{
    std::string strLibPath = "./";
    strLibPath += mstrName;
    mInst = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

    return mInst != NULL;
}

bool DynLib::unLoad()
{
    DYNLIB_UNLOAD(mInst);
    return true;
}

void* DynLib::getSymbol(const char* szProcName)
{
    return (DYNLIB_HANDLE)DYNLIB_GETSYM(mInst, szProcName);
}

bool DynLib::setLib(ILib* pLib)
{
	mLib = pLib;
	return true;
}
