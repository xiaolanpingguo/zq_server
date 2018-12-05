#include <iostream>
#include <random>
#include <variant>
#include <unordered_map>
#include <map>
#include "test_format.hpp"
//#include "baselib/core/property.h"
#include "interface_header/base/platform.h"


enum class VR_DATA_TYPE : uint32
{
	INT8, INT16, UINT16, INT32, UINT32, INT64, UINT64,
	FLOAT, DOUBLE, STRING, UUID,
};

enum class INT_TYPE : uint32
{
	FIRST = VR_DATA_TYPE::UUID
};

void f(uint32 index)
{

}
struct A
{
	A()
	{
		cout << 22 << endl;
	}
	~A()
	{
		cout << 11 << endl;
	}
	int a = 100;
};

std::unique_ptr<A[]> p;

int main()
{
	std::unique_ptr<A> p = std::make_unique<A>();
	A * p1 = p.get();
	cout << p1->a << endl;
	cout << p->a << endl;

	std::getchar();
	return 0;
}





