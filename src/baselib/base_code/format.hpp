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
		if (sizeof...(args) == 0)
		{
			return format;
		}

		auto tp = std::forward_as_tuple(std::forward<Args>(args)...);
		size_t start = 0;
		size_t pos = 0;
		std::ostringstream ss;
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

			formatImpl(ss, format.substr(start, pos - start), tp);
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

	template <typename... Args>
	static void formatImpl(std::ostringstream& ss, const std::string& item, std::tuple<Args...>& tp)
	{
		int index = 0;
		int alignment = 0;
		std::string fmt;
		std::size_t tp_size = std::tuple_size_v<std::tuple<Args...>>;

		char* endptr = nullptr;
		index = strtol(&item[0], &endptr, 10);
		if (index < 0 || index >= tp_size)
		{
			return;
		}

		if (*endptr == ',')
		{
			alignment = strtol(endptr + 1, &endptr, 10);
			if (alignment > 0)
			{
				ss << std::right << std::setw(alignment);
			}
			else if (alignment < 0)
			{
				ss << std::left << std::setw(-alignment);
			}
		}

		if (*endptr == ':')
		{
			fmt = endptr + 1;
		}

		visit(index, tp, [&](auto& item)
		{
			ss << item;
		}, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
	}
};
}
