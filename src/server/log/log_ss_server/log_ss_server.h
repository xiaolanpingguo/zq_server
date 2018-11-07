#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"

namespace zq {


class LogSSServerLib : public ILib
{
public:
	LogSSServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~LogSSServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
