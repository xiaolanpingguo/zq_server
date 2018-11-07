#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"


namespace zq {

class GameClientServerLib : public ILib
{
public:
	GameClientServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~GameClientServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
