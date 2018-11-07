#pragma once

#include "baselib/interface_header/ILib.h"
#include "baselib/interface_header/ILibManager.h"

namespace zq {


class GameServerLib : public ILib
{
public:
	GameServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~GameServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
