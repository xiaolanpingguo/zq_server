#pragma once


#include <string>
#include <map>

namespace zq {

template <class T>
class MemoryCounter
{
public:
	MemoryCounter(const std::string& strClassName)
	{
		strClassName_ = strClassName;
		s_mem_counter[strClassName_]++;
	}

	virtual ~MemoryCounter()
	{
		s_mem_counter[strClassName_]--;
	}

	std::string strClassName_;
	static std::map<std::string, int> s_mem_counter;
};

template <class T>
std::map<std::string, int> MemoryCounter<T>::s_mem_counter;


}