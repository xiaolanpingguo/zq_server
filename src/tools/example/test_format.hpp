#pragma once


#include "baselib/base_code/format.hpp"
#include <iostream>
using namespace zq;
using namespace std;


void test_format()
{
	//std::string_view s= "char*";
	const char* p = "const char*";
	int b = 1000;

	std::string test1 = fmt::format("{0}{1}dasbdbdfgdfg{3}{2}", b, b, "dwad");
	cout << test1 << endl;
	//cout << b << endl;
}






