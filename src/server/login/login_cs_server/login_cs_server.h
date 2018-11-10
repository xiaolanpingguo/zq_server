#pragma once

#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {

class LoginCSServerLib : public ILib
{
public:
	LoginCSServerLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~LoginCSServerLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
