#pragma once

#include "lib/interface_header/ILib.h"
#include "lib/interface_header/ILibManager.h"

namespace zq {


class DataAgentLib : public ILib
{
public:
	DataAgentLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~DataAgentLib()	{}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
