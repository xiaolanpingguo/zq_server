#pragma once

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {

class LoginClientServerLib : public ILib
{
public:
	LoginClientServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~LoginClientServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
