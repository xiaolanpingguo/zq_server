#include "file_process.h"
#include <iostream>
#include <chrono>
#include <cassert>
using namespace std;


int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		cout << "args error" << endl;
		assert(false);
		return 0;
	}

	std::string excelpath = argv[1];

	auto t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	FileProcess fp;
	fp.setExcelPath(excelpath);
	fp.setUTF8(false);//设置为true将支持中文显示
	fp.load();
	fp.save();

	auto t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::cout << "Generate All Done for Good!!!" << std::endl;
	std::cout << "Timespan: " << t2 - t1 << " us (" << (t2 - t1) / 1000000.0f << "s)" << std::endl;

	std::cout << "press any key to exit..." << std::endl;
	std::getchar();
	return 0;
}
