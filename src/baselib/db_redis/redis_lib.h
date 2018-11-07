#pragma once

#include "interface_header/ILib.h"
#include "interface_header/ILibManager.h"

namespace zq {


class RedisLib : public ILib
{
public:
	RedisLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~RedisLib()	{}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
