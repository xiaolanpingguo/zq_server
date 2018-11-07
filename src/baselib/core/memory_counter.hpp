#pragma once

#include <iostream>
#include <string>
#include <map>
#include "interface_header/platform.h"

namespace zq {

template <class T>
class MemoryCounter
{
private:
	MemoryCounter() {}
public:
	MemoryCounter(const std::string& strClassName)
	{
		strClassName_ = strClassName;
		mxCounter[strClassName_]++;
	}

	virtual ~MemoryCounter()
	{
		mxCounter[strClassName_]--;
	}

	std::string strClassName_;
	static std::map<std::string, int> mxCounter;
};

template <class T>
std::map<std::string, int> MemoryCounter<T>::mxCounter;


}