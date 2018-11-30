#pragma once  

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {

class Test2Lib : public ILib
{
public:
	Test2Lib(ILibManager* p)
	{
		libManager_ = p;
		std::cout << "build!!!" << std::endl;
	}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}


