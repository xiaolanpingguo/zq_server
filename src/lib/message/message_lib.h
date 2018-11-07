#pragma once

#include "lib/interface_header/ILib.h"
#include "lib/interface_header/ILibManager.h"

namespace zq {

class MessageLib : public ILib
{
public:
	MessageLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~MessageLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
