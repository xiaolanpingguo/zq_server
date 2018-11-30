#pragma once


#define ZQ_PLATFORM_WIN			0
#define ZQ_PLATFORM_UNIX		1
#define ZQ_PLATFORM_APPLE		2

#define UNIX_FLAVOUR_LINUX		1
#define UNIX_FLAVOUR_BSD		2
#define UNIX_FLAVOUR_OTHER		3
#define UNIX_FLAVOUR_OSX		4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define ZQ_PLATFORM ZQ_PLATFORM_WIN
#elif defined( __INTEL_COMPILER )
#  define ZQ_PLATFORM ZQ_PLATFORM_INTEL
#elif defined( __APPLE_CC__ )
#  define ZQ_PLATFORM ZQ_PLATFORM_APPLE
#else
#  define ZQ_PLATFORM ZQ_PLATFORM_UNIX
#endif

#define ZQ_COMPILER_MICROSOFT 0
#define ZQ_COMPILER_GNU	   1
#define ZQ_COMPILER_BORLAND   2
#define ZQ_COMPILER_INTEL     3
#define ZQ_COMPILER_CLANG     4

#ifdef _MSC_VER
#  define ZQ_COMPILER ZQ_COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define ZQ_COMPILER ZQ_COMPILER_INTEL
#elif defined( __BORLANDC__ )
#  define ZQ_COMPILER ZQ_COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define ZQ_COMPILER ZQ_COMPILER_GNU
#elif defined( __clang__ )
#  define ZQ_COMPILER ZQ_COMPILER_CLANG

#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX || ZQ_PLATFORM == ZQ_PLATFORM_APPLE
#ifdef HAVE_DARWIN
#define ZQ_PLATFORM_TEXT "MacOSX"
#define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#else
#ifdef USE_KQUEUE
#define ZQ_PLATFORM_TEXT "FreeBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#ifdef USE_KQUEUE_DFLY
#define ZQ_PLATFORM_TEXT "DragonFlyBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#define ZQ_PLATFORM_TEXT "Linux"
#define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#endif
#endif
#endif
#endif


#if defined(_DEBUG) || defined(DEBUG)
#define ZQ_DEBUG_MODE 1
#endif

#ifndef X64
#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( __amd64 ) || defined( __LP64__ )
#define X64
#endif
#endif

#ifdef X64
#define ZQ_ARCH "X64"
#else
#define ZQ_ARCH "X86"
#endif


// 字节序
#define ZQ_LITTLE_ENDIAN							0
#define ZQ_BIG_ENDIAN								1
#if !defined(ZQ_ENDIAN)
//#define ZQ_ENDIAN
#  if defined (USE_BIG_ENDIAN)
#    define ZQ_ENDIAN ZQ_BIG_ENDIAN
#  else 
#    define ZQ_ENDIAN ZQ_LITTLE_ENDIAN
#  endif 
#endif



#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#define ZQ_PLATFORM_TEXT "Win32"
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501	//引入boost asio后会有的警告
#endif
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4049)
#pragma warning(disable:4217)
#define SIGHUP	1
#define SIGINT	2
#define SIGQUIT 3
#define SIGUSR1 10
#define SIGPIPE 13
#define SIGCHLD 17
#define SIGSYS	32
#endif


#define USE_STATIC_LIB	0
#if USE_STATIC_LIB == 1
// Linux compilers don't have symbol import/export directives.
#       define ZQ_EXPORT
#       define ZQ_IMPORT
#else 
#  if ZQ_COMPILER == COMPILER_MICROSOFT
#    define ZQ_EXPORT __declspec(dllexport)
#    define ZQ_IMPORT __declspec(dllimport)
#  elif ZQ_COMPILER == COMPILER_GNU
#    define ZQ_EXPORT __attribute__((visibility("default")))
#    define ZQ_IMPORT
#  else
#    error compiler not supported!
#  endif
#endif


// 取消min和max的定义
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN

#undef min
#undef max
#define NOMINMAX	  // windows.h中有min和max的定义

#endif




// 先把平台的相关宏定义好再包含头文件
// 否则某些功能可能失效，比如zq_log.hpp的std::max
#include <stdint.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <thread> // std::this_thread::sleep_for
#include <stdarg.h> // va_end()
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#else
#include <sys/time.h>				  
#include <unistd.h>
#endif
#include "baselib/base_code/lexical_cast.hpp"
#include "baselib/base_code/zq_log.hpp"


namespace zq{


#ifndef TCHAR
#ifdef _UNICODE
	typedef wchar_t												TCHAR;
#else
	typedef char												TCHAR;
#endif
#endif

/* Use correct types for x64 platforms, too */
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
typedef signed __int64											int64;
typedef signed __int32											int32;
typedef signed __int16											int16;
typedef signed __int8											int8;
typedef unsigned __int64										uint64;
typedef unsigned __int32										uint32;
typedef unsigned __int16										uint16;
typedef unsigned __int8											uint8;
#define PRI64													"lld"
#define PRIu64													"llu"
#define PRIx64													"llx"
#define PRIX64													"llX"
#define PRIzu													"lu"
#define PRIzd													"ld"
#define PRTime													PRI64
#else
typedef int64_t													int64;
typedef int32_t													int32;
typedef int16_t													int16;
typedef int8_t													int8;
typedef uint64_t												uint64;
typedef uint32_t												uint32;
typedef uint16_t												uint16;
typedef uint8_t													uint8;
#endif


#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#define SPRINTF sprintf_s
#define STRICMP stricmp
#else
#define SPRINTF snprintf
#define STRICMP strcasecmp
#endif



#if ZQ_ENDIAN == ZQ_LITTLE_ENDIAN
/* accessing individual bytes (int8) and words (int16) within
 * words and long words (int32).
 * Macros ending with W deal with words, L macros deal with longs
 */
/// Returns the high byte of a word.
#define HIBYTEW(b)		(((b) & 0xff00) >> 8)
/// Returns the low byte of a word.
#define LOBYTEW(b)		( (b) & 0xff)

/// Returns the high byte of a long.
#define HIBYTEL(b)		(((b) & 0xff000000L) >> 24)
/// Returns the low byte of a long.
#define LOBYTEL(b)		( (b) & 0xffL)

/// Returns byte 0 of a long.
#define BYTE0L(b)		( (b) & 0xffL)
/// Returns byte 1 of a long.
#define BYTE1L(b)		(((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)		(((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a long.
#define BYTE3L(b)		(((b) & 0xff000000L) >> 24)

/// Returns the high word of a long.
#define HIWORDL(b)		(((b) & 0xffff0000L) >> 16)
/// Returns the low word of a long.
#define LOWORDL(b)		( (b) & 0xffffL)

/**
 *	This macro takes a dword ordered 0123 and reorder it to 3210.
 */
#define SWAP_DW(a)	  ( (((a) & 0xff000000)>>24) |	\
						(((a) & 0xff0000)>>8) |		\
						(((a) & 0xff00)<<8) |		\
						(((a) & 0xff)<<24) )

#else
/* big endian macros go here */ 
#endif


#define GET_CLASS_NAME(className) (#className)
#define MAX_NAME 256	
#define MAX_IP 256
#define MAX_BUF 256

#ifndef MAX_PATH
#define MAX_PATH 260
#endif


#define SAFE_RELEASE(i)										\
	if (i)													\
		{													\
			delete i;										\
			i = NULL;										\
		}

#define SAFE_RELEASE_ARRAY(i)								\
	if (i)													\
		{													\
			delete[] i;										\
			i = NULL;										\
		}



inline bool isPlatformLittleEndian()
{
   int n = 1;
   return *((char*)&n) ? true : false;
}

template<typename DTYPE>
bool strto(const std::string& strValue, DTYPE& nValue)
{
	try
	{
		nValue = lexical_cast<DTYPE>(strValue);
		return true;
	}
	catch (...)
	{
		return false;
	}

	return false;
	}

inline bool isZeroFloat(const float fValue, float epsilon = 1e-6)
{
	return std::abs(fValue) <= epsilon;
}

inline bool isZeroDouble(const double dValue, double epsilon = 1e-15)
{
	return std::abs(dValue) <= epsilon;
}

//millisecond
inline int64 getTimestampMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//second
inline int64 getTimestampS()
{
	return getTimestampMs() / 1000;
}

inline uint32 getMSTime()
{
	using namespace std::chrono;
	static const steady_clock::time_point ApplicationStartTime = steady_clock::now();
	return uint32(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
}

inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
{
	if (oldMSTime > newMSTime)
		return (0xFFFFFFFF - oldMSTime) + newMSTime;
	else
		return newMSTime - oldMSTime;
}

inline uint32 GetMSTimeDiffToNow(uint32 oldMSTime)
{
	return getMSTimeDiff(oldMSTime, getMSTime());
}


inline void sleep(uint32 ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


inline void Assert(char const* file, int line, char const* function, char const* message)
{
	fprintf(stderr, "\n%s:%i in %s ASSERTION FAILED:\n  %s\n",
		file, line, function, message);
	*((volatile int*)nullptr) = 0;
	exit(1);
}

inline void Assert(char const* file, int line, char const* function, char const* message, char const* format, ...)
{
	va_list args;
	va_start(args, format);

	fprintf(stderr, "\n%s:%i in %s ASSERTION FAILED:\n  %s ", file, line, function, message);
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	va_end(args);
	*((volatile int*)nullptr) = 0;
	exit(1);
}

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#define ASSERT(cond, ...) ASSERT_BEGIN do { if (!(cond)) Assert(__FILE__, __LINE__, __FUNCTION__, #cond, ##__VA_ARGS__); } while(0) ASSERT_END


namespace ByteConverter
{
	template<size_t T>
	inline void convert(char *val)
	{
		std::swap(*val, *(val + T - 1));
		convert<T - 2>(val + 1);
	}

	template<> inline void convert<0>(char *) { }
	template<> inline void convert<1>(char *) { }           // ignore central byte

	template<typename T> inline void apply(T *val)
	{
		convert<sizeof(T)>((char *)(val));
	}
}

#define ZQ_LITTLEENDIAN 0
#define ZQ_BIGENDIAN    1
#if !defined(ZQ_ENDIAN)
#  if defined (USE_BIG_ENDIAN)
#    define ZQ_ENDIAN ZQ_BIGENDIAN
#  else 
#    define ZQ_ENDIAN ZQ_LITTLEENDIAN
#  endif 
#endif

#if ZQ_ENDIAN == ZQ_BIGENDIAN
template<typename T> inline void EndianConvert(T& val) { ByteConverter::apply<T>(&val); }
template<typename T> inline void EndianConvertReverse(T&) { }
template<typename T> inline void EndianConvertPtr(void* val) { ByteConverter::apply<T>(val); }
template<typename T> inline void EndianConvertPtrReverse(void*) { }
#else
template<typename T> inline void EndianConvert(T&) { }
template<typename T> inline void EndianConvertReverse(T& val) { ByteConverter::apply<T>(&val); }
template<typename T> inline void EndianConvertPtr(void*) { }
template<typename T> inline void EndianConvertPtrReverse(void* val) { ByteConverter::apply<T>(val); }
#endif

template<typename T> void EndianConvert(T*);         // will generate link error
template<typename T> void EndianConvertReverse(T*);  // will generate link error

inline void EndianConvert(uint8&) { }
inline void EndianConvert(int8&) { }
inline void EndianConvertReverse(uint8&) { }
inline void EndianConvertReverse(int8&) { }


}