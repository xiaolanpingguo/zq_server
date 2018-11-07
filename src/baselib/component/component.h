#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"

namespace zq {


class ComponentLib : public ILib
{
public:
	ComponentLib(ILibManager* p)
	{
		libManager_ = p;
	}  

	virtual ~ComponentLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
