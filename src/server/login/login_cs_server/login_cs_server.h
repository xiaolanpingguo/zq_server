#pragma once

#include "baselib/interface_header/ILib.h"
#include "baselib/interface_header/ILibManager.h"


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
