#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"


namespace zq {


class MasterSSServerLib : public ILib
{
public:
	MasterSSServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~MasterSSServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
