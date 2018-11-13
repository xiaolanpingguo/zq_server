#include "util.h"
#include "dependencies/utf8cpp/utf8.h"
#include <functional>    
#include <algorithm>
#include <cctype>	//std::isspace
#include <string.h> // memcpy
#include <time.h>	// localtime_s
#include <stdarg.h>	// va_start,linux
#ifdef _WIN32 
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace zq {
namespace util {

Tokenizer::Tokenizer(const std::string &src, const char sep, int vectorReserve, bool keepEmptyStrings)
{
	m_str = new char[src.length() + 1];
	memcpy(m_str, src.c_str(), src.length() + 1);

	if (vectorReserve)
		m_storage.reserve(vectorReserve);

	char* posold = m_str;
	char* posnew = m_str;

	for (;;)
	{
		if (*posnew == sep)
		{
			if (keepEmptyStrings || posold != posnew)
				m_storage.push_back(posold);

			posold = posnew + 1;
			*posnew = '\0';
		}
		else if (*posnew == '\0')
		{
			// Hack like, but the old code accepted these kind of broken strings,
			// so changing it would break other things
			if (posold != posnew)
				m_storage.push_back(posold);

			break;
		}

		++posnew;
	}
}

Tokenizer::~Tokenizer()
{
	delete[] m_str;
}

int bytes2string(unsigned char *src, int srcsize, unsigned char *dst, int dstsize)
{
	if (dst != NULL)
	{
		*dst = 0;
	}

	if (src == NULL || srcsize <= 0 || dst == NULL || dstsize <= srcsize * 2)
	{
		return 0;
	}

	const char szTable[] = "0123456789ABCDEF";

	for (int i = 0; i < srcsize; ++i)
	{
		*dst++ = szTable[src[i] >> 4];
		*dst++ = szTable[src[i] & 0x0f];
	}

	*dst = 0;
	return  srcsize * 2;
}

int string2bytes(unsigned char* src, unsigned char* dst, int dstsize)
{
	if (src == NULL)
		return 0;

	int iLen = (int)strlen((char *)src);
	if (iLen <= 0 || iLen % 2 != 0 || dst == NULL || dstsize < iLen / 2)
	{
		return 0;
	}

	iLen /= 2;
	str_toupper((char *)src);
	for (int i = 0; i < iLen; ++i)
	{
		int iVal = 0;
		unsigned char *pSrcTemp = src + i * 2;
		sscanf((char *)pSrcTemp, "%02x", &iVal);
		dst[i] = (unsigned char)iVal;
	}

	return iLen;
}

std::string toLower(const std::string& str) {
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), tolower);
	return t;
}

std::string toUpper(const std::string& str) {
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), toupper);
	return t;
}

int strReplace(std::string& str, const std::string& pattern, const std::string& newpat)
{
	int count = 0;
	const size_t nsize = newpat.size();
	const size_t psize = pattern.size();

	for (size_t pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
	{
		str.replace(pos, psize, newpat);
		count++;
	}

	return count;
}

int wstrReplace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat)
{
	int count = 0;
	const size_t nsize = newpat.size();
	const size_t psize = pattern.size();

	for (size_t pos = str.find(pattern, 0);
		pos != std::wstring::npos;
		pos = str.find(pattern, pos + nsize))
	{
		str.replace(pos, psize, newpat);
		count++;
	}

	return count;
}


std::vector<std::string> zq_splits(const std::string& s, const std::string& delim, const bool keep_empty)
{
	std::vector<std::string> result;

	if (delim.empty())
	{
		result.push_back(s);
		return result;
	}

	std::string::const_iterator substart = s.begin(), subend;

	while (1)
	{
		subend = std::search(substart, s.end(), delim.begin(), delim.end());
		std::string temp(substart, subend);
		if (keep_empty || !temp.empty())
		{
			result.push_back(temp);
		}

		if (subend == s.end())
		{
			break;
		}

		substart = subend + delim.size();
	}

	return result;
}

std::string& ltrim(std::string &s)
{
	//s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {return !std::isspace(ch); }));
	return s;
}

std::string& rtrim(std::string &s)
{
	//s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch); }).base(), s.end());
	return s;
}

std::string trim(std::string s)
{
	return ltrim(rtrim(s));
}

bool utf8toWStr(const std::string& utf8str, std::wstring& wstr)
{
	try
	{
		if (size_t len = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size()))
		{
			wstr.resize(len);
			utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
		}
	}
	catch (std::exception)
	{
		wstr.clear();
		return false;
	}

	return true;
}

bool utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize)
{
	try
	{
		size_t len = utf8::distance(utf8str, utf8str + csize);
		if (len > wsize)
		{
			if (wsize > 0)
				wstr[0] = L'\0';
			wsize = 0;
			return false;
		}

		wsize = len;
		utf8::utf8to16(utf8str, utf8str + csize, wstr);
		wstr[len] = L'\0';
	}
	catch (std::exception)
	{
		if (wsize > 0)
			wstr[0] = L'\0';
		wsize = 0;
		return false;
	}

	return true;
}

bool wstrToUtf8(std::wstring const& wstr, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(wstr.size() * 4);                     // allocate for most long case

		if (wstr.size())
		{
			char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));                // remove unused tail
		}
		utf8str = utf8str2;
	}
	catch (std::exception)
	{
		utf8str.clear();
		return false;
	}

	return true;
}

void wstrToLower(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), wcharToLower);
}

bool utf8ToConsole(const std::string& utf8str, std::string& conStr)
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	std::wstring wstr;
	if (!utf8toWStr(utf8str, wstr))
		return false;

	conStr.resize(wstr.size());
	CharToOemBuffW(&wstr[0], &conStr[0], uint32_t(wstr.size()));
#else
	// not implemented yet
	conStr = utf8str;
#endif

	return true;
}

bool consoleToUtf8(const std::string& conStr, std::string& utf8str)
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	std::wstring wstr;
	wstr.resize(conStr.size());
	OemToCharBuffW(&conStr[0], &wstr[0], uint32_t(conStr.size()));

	return wstrToUtf8(wstr, utf8str);
#else
	// not implemented yet
	utf8str = conStr;
	return true;
#endif
}

bool utf8FitTo(const std::string& str, std::wstring const& search)
{
	std::wstring temp;

	if (!utf8toWStr(str, temp))
		return false;

	// converting to lower case
	wstrToLower(temp);

	if (temp.find(search) == std::wstring::npos)
		return false;

	return true;
}

void utf8printf(FILE* out, const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vutf8printf(out, str, &ap);
	va_end(ap);
}

void vutf8printf(FILE* out, const char *str, va_list* ap)
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	char temp_buf[32 * 1024];
	wchar_t wtemp_buf[32 * 1024];

	size_t temp_len = vsnprintf(temp_buf, 32 * 1024, str, *ap);
	//vsnprintf returns -1 if the buffer is too small
	if (temp_len == size_t(-1))
		temp_len = 32 * 1024 - 1;

	size_t wtemp_len = 32 * 1024 - 1;
	utf8toWStr(temp_buf, temp_len, wtemp_buf, wtemp_len);

	CharToOemBuffW(&wtemp_buf[0], &temp_buf[0], uint32_t(wtemp_len + 1));
	fprintf(out, "%s", temp_buf);
#else
	vfprintf(out, str, *ap);
#endif
}

bool utf8ToUpperOnlyLatin(std::string& utf8String)
{
	std::wstring wstr;
	if (!utf8toWStr(utf8String, wstr))
		return false;

	std::transform(wstr.begin(), wstr.end(), wstr.begin(), wcharToUpperOnlyLatin);

	return wstrToUtf8(wstr, utf8String);
}

uint32_t createPIDFile(std::string const& filename)
{
	FILE* pid_file = fopen(filename.c_str(), "w");
	if (pid_file == nullptr)
		return 0;

	uint32_t pid = getPid();

	fprintf(pid_file, "%u", pid);
	fclose(pid_file);

	return pid;
}

uint32_t getPid()
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	DWORD pid = GetCurrentProcessId();
#else
	pid_t pid = getpid();
#endif

	return uint32_t(pid);
}


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
struct tm* localtime_r(time_t const* time, struct tm *result)
{
	localtime_s(result, time);
	return result;
}
#endif



}}
