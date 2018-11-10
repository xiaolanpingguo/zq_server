#pragma once

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"

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
