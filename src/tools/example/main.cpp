#include <iostream>
#include <random>
#include <variant>
#include <unordered_map>
#include <map>
#include "test_format.hpp"
#include "interface_header/base/platform.h"
#include "baselib/message/game_db_logic.pb.h"
#include "baselib/base_code/reflection/json.hpp"
#pragma comment(lib, "libprotobuf_d.lib")	

struct A
{
	int a = 20;
	float b = 30.f;
};
REFLECTION(A, a, b) //define meta data

struct B
{
	int b = 300;
	std::array<A, 1> a;
};
REFLECTION(B, b, a) //define meta data

struct person
{
	std::string  name;
	float          age  =10.1f;
	B b;
};

REFLECTION(person, name ,age, b) //define meta data


int main()
{
	person p;

	B b;
	b.a[0] = { 10, 90.1f };
	b.b = 200;
	p.age = 28;
	p.name = "abc";
	p.b = b;

	zq::reflection::string_stream ss;
	zq::reflection::json::to_json(ss, p);

	std::cout << ss.str() << std::endl;

	const char * json = "{ \"name\" : \"tom\", \"age\" : 28}";

	person p1;
	zq::reflection::json::from_json(p1, ss.str().c_str());

	std::getchar();
	return 0;
}





