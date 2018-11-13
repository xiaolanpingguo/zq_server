#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace zq {
namespace util {

#if defined( unix )

#define zq_isnan isnan
#define zq_isinf isinf
#define zq_snprintf snprintf
#define zq_vsnprintf vsnprintf
#define zq_vsnwprintf vsnwprintf
#define zq_snwprintf swprintf
#define zq_stricmp strcasecmp
#define zq_strnicmp strncasecmp
#define zq_fileno fileno
#define zq_va_copy va_copy
#else
#define zq_isnan _isnan
#define zq_isinf(x) (!_finite(x) && !_isnan(x))
#define zq_snprintf _snprintf
#define zq_vsnprintf _vsnprintf
#define zq_vsnwprintf _vsnwprintf
#define zq_snwprintf _snwprintf
#define zq_stricmp _stricmp
#define zq_strnicmp _strnicmp
#define zq_fileno _fileno
#define zq_va_copy( dst, src) dst = src

#define strtoq   _strtoi64
#define strtouq  _strtoui64
#define strtoll  _strtoi64
#define strtoull _strtoui64
#define atoll    _atoi64

#endif // unix


// 去掉左右两边的空格
std::string& ltrim(std::string &s);
std::string& rtrim(std::string &s);
std::string trim(std::string s);

int strReplace(std::string& str, const std::string& pattern, const std::string& newpat);
int wstrReplace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat);

std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);

/*
	转换为大写
*/
inline char* str_toupper(char* s)
{
	if (s == nullptr)
		return nullptr;

	while (*s)
	{
		*s = toupper((unsigned char)*s);
		s++;
	};

	return s;
}

/*
	转换为小写
*/
inline char* str_tolower(char* s)
{
	if (s == nullptr)
		return nullptr;

	while (*s)
	{
		*s = tolower((unsigned char)*s);
		s++;
	};

	return s;
}

template<typename T>
inline void split(const std::basic_string<T>& s, T c, std::vector<std::basic_string<T>>& v)
{
	if (s.size() == 0)
		return;

	typename std::basic_string<T>::size_type i = 0;
	typename std::basic_string<T>::size_type j = s.find(c);

	while (j != std::basic_string<T>::npos)
	{
		std::basic_string<T> buf = s.substr(i, j - i);

		if (buf.size() > 0)
			v.push_back(buf);

		i = ++j; j = s.find(c, j);
	}

	if (j == std::basic_string<T>::npos)
	{
		std::basic_string<T> buf = s.substr(i, s.length() - i);
		if (buf.size() > 0)
			v.push_back(buf);
	}
}

std::vector< std::string > zq_splits(const std::string& s, const std::string& delim, const bool keep_empty = true);

int bytes2string(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen);
int string2bytes(unsigned char* szSrc, unsigned char* pDst, int nDstMaxLen);



// vector<string>之类的容易使用 std::find_if 来查找是否存在某个字符串
template<typename T>
class find_vec_string_exist_handle
{
public:
	find_vec_string_exist_handle(std::basic_string< T > str)
		: str_(str) {}

	bool operator()(const std::basic_string< T > &strSrc)
	{
		return strSrc == str_;
	}

	bool operator()(const T* strSrc)
	{
		return strSrc == str_;
	}
private:
	std::basic_string< T > str_;
};


// Tokenizer class
class Tokenizer
{
public:
	typedef std::vector<char const*> StorageType;
	typedef StorageType::size_type size_type;
	typedef StorageType::const_iterator const_iterator;
	typedef StorageType::reference reference;
	typedef StorageType::const_reference const_reference;

public:
	Tokenizer(const std::string &src, const char sep, int vectorReserve = 0, bool keepEmptyStrings = true);
	~Tokenizer();

	const_iterator begin() const { return m_storage.begin(); }
	const_iterator end() const { return m_storage.end(); }
	size_type size() const { return m_storage.size(); }
	reference operator [] (size_type i) { return m_storage[i]; }
	const_reference operator [] (size_type i) const { return m_storage[i]; }

private:
	char* m_str;
	StorageType m_storage;
};


inline bool isBasicLatinCharacter(wchar_t wchar)
{
	if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
		return true;
	if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
		return true;
	return false;
}

inline wchar_t wcharToLower(wchar_t wchar)
{
	if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
		return wchar_t(uint16_t(wchar) + 0x0020);
	if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
		return wchar_t(uint16_t(wchar) + 0x0020);
	if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
		return wchar_t(uint16_t(wchar) + 0x0020);
	if (wchar >= 0x0100 && wchar <= 0x012E)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
	{
		if (wchar % 2 == 0)
			return wchar_t(uint16_t(wchar) + 0x0001);
	}
	if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
		return wchar_t(0x00DF);
	if (wchar == 0x0401)                                     // CYRILLIC CAPITAL LETTER IO
		return wchar_t(0x0451);
	if (wchar >= 0x0410 && wchar <= 0x042F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
		return wchar_t(uint16_t(wchar) + 0x0020);

	return wchar;
}

inline wchar_t wcharToUpper(wchar_t wchar)
{
	if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
		return wchar_t(uint16_t(wchar) - 0x0020);
	if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
		return wchar_t(0x1E9E);
	if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
		return wchar_t(uint16_t(wchar) - 0x0020);
	if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
		return wchar_t(uint16_t(wchar) - 0x0020);
	if (wchar >= 0x0101 && wchar <= 0x012F)                  // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
	{
		if (wchar % 2 == 1)
			return wchar_t(uint16_t(wchar) - 0x0001);
	}
	if (wchar >= 0x0430 && wchar <= 0x044F)                  // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
		return wchar_t(uint16_t(wchar) - 0x0020);
	if (wchar == 0x0451)                                     // CYRILLIC SMALL LETTER IO
		return wchar_t(0x0401);

	return wchar;
}

inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
{
	return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}

// utf-8	
bool utf8toWStr(const std::string& utf8str, std::wstring& wstr);
bool utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize);
bool wstrToUtf8(std::wstring const& wstr, std::string& utf8str);
void wstrToLower(std::wstring& str);

bool utf8ToConsole(const std::string& utf8str, std::string& conStr);
bool consoleToUtf8(const std::string& conStr, std::string& utf8str);
bool utf8FitTo(const std::string& str, std::wstring const& search);
void utf8printf(FILE* out, const char *str, ...);
void vutf8printf(FILE* out, const char *str, va_list* ap);
bool utf8ToUpperOnlyLatin(std::string& utf8String);

// 获得进程pid
uint32_t createPIDFile(const std::string& filename);
uint32_t getPid();

struct tm* localtime_r(time_t const* time, struct tm *result);


}}



