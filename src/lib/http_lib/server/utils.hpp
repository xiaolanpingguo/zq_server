#pragma once


#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <locale>	 // std::wstring_convert
#include <codecvt>	 // std::codecvt_utf8

namespace zq {
	


inline std::string get_extension(const std::string& name)
{
	size_t pos = name.rfind('.');
	if (pos == std::string::npos)
	{
		return {};
	}

	return name.substr(pos);
}

inline bool iequal(const char *s, size_t l, const char *t)
{
	if (strlen(t) != l)
		return false;

	for (size_t i = 0; i < l; i++) {
		if (std::tolower(s[i]) != std::tolower(t[i]))
			return false;
	}

	return true;
}

inline std::string& ltrim(std::string& s)
{
	//s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {return !std::isspace(ch); }));
	return s;
}

inline std::string& rtrim(std::string& s)
{
	//s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch); }).base(), s.end());
	return s;
}

inline std::string trim(std::string s)
{
	return ltrim(rtrim(s));
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

inline std::string to_hex_string(std::size_t value)
{
	std::ostringstream stream;
	stream << std::hex << value;
	return stream.str();
}


inline static std::string url_encode(const std::string &value) noexcept
{
	static auto hex_chars = "0123456789ABCDEF";

	std::string result;
	result.reserve(value.size()); // Minimum size of result

	for (auto &chr : value)
	{
		if (!((chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '-' || chr == '.' || chr == '_' || chr == '~'))
			result += std::string("%") + hex_chars[static_cast<unsigned char>(chr) >> 4] + hex_chars[static_cast<unsigned char>(chr) & 15];
		else
			result += chr;
	}

	return result;
}

inline static std::string url_decode(const std::string &value) noexcept
{
	std::string result;
	result.reserve(value.size() / 3 + (value.size() % 3)); // Minimum size of result

	for (std::size_t i = 0; i < value.size(); ++i)
	{
		auto &chr = value[i];
		if (chr == '%' && i + 2 < value.size())
		{
			auto hex = value.substr(i + 1, 2);
			auto decoded_chr = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
			result += decoded_chr;
			i += 2;
		}
		else if (chr == '+')
			result += ' ';
		else
			result += chr;
	}

	return result;
}

inline static std::string u8wstring_to_string(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

inline static std::wstring u8string_to_wstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return conv.from_bytes(str);
}

inline static std::string get_string_by_urldecode(const std::string& content)
{
	return url_decode(std::string(content.data(), content.size()));
}

inline static bool is_url_encode(const std::string& str)
{
	return str.find("%") != std::string::npos || str.find("+") != std::string::npos;
}


}


