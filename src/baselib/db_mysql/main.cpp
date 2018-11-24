#ifdef _MSC_VER
#include <WinSock2.h>
#endif

#include <iostream>
#include <thread>
#include "dbng.hpp"
#include "mysql.hpp"
#include <iguana/reflection.hpp>
#pragma comment(lib, "libmysql.lib")	
#pragma comment(lib, "base_code_d.lib")




struct person
{
    int id;
    std::string name;
    int age;
	int number;
};
REFLECTION(person, id, name, age, number)

using namespace zq;
const char* ip = "127.0.0.1"; //your database ip



struct A
{

};
int main(int argc, char* argv[])
{
	person p = { 1, "test1", 100, 200};
	//person p1 = { 2, "test2", 2 };
	//person p2 = { 3, "test3", 3 };
	//std::vector<person> v{ p1, p2 };

	dbng<MysqlConnection> mysql;
	mysql.connect("127.0.0.1", "zq", "8292340", "test_db");

	////primary_key key{ "id" };
	////bool b = mysql.create_datatable<person>(key);

	//mysql.insert(p);
	//mysql.insert(v);
	auto f = [](QueryResult result)
	{
		if (result == nullptr)
		{
			return;
		}

		for (const auto& ele : std::static_pointer_cast<ResultDerive<person>>(result)->vecResult_)
		{
			std::cout << ele.id << std::endl;
			std::cout << ele.name << std::endl;
			std::cout << ele.age << std::endl;
			std::cout << ele.number << std::endl;
		}
	};

	mysql.async_query<person>(std::move(f));

	while (1)
	{
		mysql.run();
	}


	////int a = mysql.update(p, "id");
	////mysql.update(v);

	//auto result = mysql.query<person>(); //vector<person>
	//for (auto& person : result) 
	//{
	//	std::cout << person.id << " " << person.name << " " << person.age << std::endl;
	//}

	//mysql.delete_records<person>();

	////transaction
	//mysql.begin();
	//for (int i = 0; i < 10; ++i) {
	//	person s = { i, "tom", 19 };
	//	if (!mysql.insert(s)) {
	//		mysql.rollback();
	//		return -1;
	//	}
	//}
	//mysql.commit();

	std::getchar();
	return 0;
}