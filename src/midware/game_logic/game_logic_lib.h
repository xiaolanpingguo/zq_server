#pragma once


#include "interface_header/base/ILib.h"

namespace zq {


class GameLogicLib : public ILib
{
public:
	GameLogicLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~GameLogicLib()	{}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
