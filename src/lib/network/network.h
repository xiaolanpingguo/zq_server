#pragma once

#include "lib/interface_header/ILib.h"
#include "lib/interface_header/ILibManager.h"


namespace zq {

class NetworkLib : public ILib
{
public:
	NetworkLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
