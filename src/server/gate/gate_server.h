#pragma once

#include "baselib/interface_header/ILib.h"
#include "baselib/interface_header/ILibManager.h"


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
