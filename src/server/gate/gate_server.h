#pragma once

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {

class GateServerLib : public ILib
{
public:
	GateServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~GateServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
