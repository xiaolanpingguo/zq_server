#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"

namespace zq {


class GameCSServerLib : public ILib
{
public:
	GameCSServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~GameCSServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
