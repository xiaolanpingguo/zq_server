#pragma once


#include "baselib/base_code/format.hpp"
#include <iostream>
#include <thread>
using namespace zq;
using namespace std;

static void f1()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string test1 = fmt::format("{0}--{1}--{2}", 123, "abc", 0.987);
		cout << test1 << endl;
	}
}

static void f2()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string test1 = fmt::format("{0}--{1}--{2}", 456, "qwe", 0.987);
		cout << test1 << endl;
	}
}

static void f3()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string test1 = fmt::format("{1}--{3}--{}--{}--{}--iop", 789, "tyu", 0.987);
		cout << test1 << endl;
	}
}


void test_format()
{
	//std::thread thr1{ f1 };
	//std::thread thr2{ f2 };
	std::thread thr3{ f3 };
	//thr1.join();
	//thr2.join();
	thr3.join();
}






