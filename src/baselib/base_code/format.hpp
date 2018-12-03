#pragma once


#include <sstream>
#include <tuple>
#include <iomanip> // std::setw


namespace zq{

struct fmt
{
	template <typename... Args>
	static std::string format(const std::string& format, Args&&... args)
	{
		size_t tp_size = sizeof...(args);
		if (tp_size <= 0)
		{
			return format;
		}

		auto tp = std::forward_as_tuple(std::forward<Args>(args)...);
		size_t start = 0;
		size_t pos = 0;
		std::ostringstream ss;
		int i = 0;
		while (1)
		{
			pos = format.find('{', start);
			if (pos == std::string::npos)
			{
				ss << format.substr(start);
				break;
			}

			ss << format.substr(start, pos - start);
			if (format[pos + 1] == '{')
			{
				ss << '{';
				start = pos + 2;
				continue;
			}

			start = pos + 1;
			pos = format.find('}', start);
			if (pos == std::string::npos)
			{
				ss << format.substr(start - 1);
				break;
			}

			// 没有手动指定位置，默认按照从参数左到右进行格式
			int index = 0;
			std::string sub_str = format.substr(start, pos - start);
			if (sub_str.empty())
			{
				index = i;
				i++;
			}
			else
			{
				// 手动指定了位置, 如果超出了范围，就默认按第一个参数解析
				char* endptr = nullptr;
				index = strtol(sub_str.c_str(), &endptr, 10);
				if (index < 0 || index >= tp_size)
				{
					index = 0;
				}
			}

			visit(index, tp, [&](auto& item)
			{
				ss << item;
			}, std::make_index_sequence<sizeof...(args)>{});

			start = pos + 1;
		}

		return ss.str();
	}
private:

	//template <size_t Idx, typename T, typename F>
	//static void visit(T& tup, size_t i, F&& f)
	//{
	//	if (i == Idx)
	//		std::forward<F>(f)(std::get<Idx>(tup));
	//}

	//template <typename Tuple, typename F, std::size_t... Idx>
	//static void visit(std::size_t i, Tuple& t, F&& f, std::index_sequence<Idx...>)
	//{
	//	(visit<Idx>(t, i, std::forward<F>(f)), ...);
	//}

	template <class Tuple, class F, std::size_t...Idx>
	static void visit(std::size_t i, Tuple& t, F&& f, std::index_sequence<Idx...>)
	{
		((i == Idx && ((std::forward<F>(f)(std::get<Idx>(t))), false)), ...);
	}
};
}
