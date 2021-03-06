#pragma once

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"

namespace zq {


class WorldSSServerLib : public ILib
{
public:
	WorldSSServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~WorldSSServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
