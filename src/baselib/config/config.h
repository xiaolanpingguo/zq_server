#pragma once

#include "baselib/interface_header/ILib.h"
#include "baselib/interface_header/ILibManager.h"


namespace zq {

class ConfigLib : public ILib
{
public:
	ConfigLib(ILibManager* p)
	{
		libManager_ = p;
	}

	~ConfigLib();

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
