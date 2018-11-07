#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"

namespace zq {


class TestServerLib : public ILib
{
public:
	TestServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~TestServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
