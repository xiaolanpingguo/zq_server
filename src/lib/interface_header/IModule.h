#pragma once

#include <string>
#include "ILibManager.h"
#include "lib/core/map_ex.hpp"
#include "lib/core/data_list.hpp"


namespace zq {

class IModule
{
public:
	IModule()
	{
	}

	virtual ~IModule() {}

	virtual bool init()
	{
		return true;
	}

	virtual bool initEnd()
	{
		return true;
	}

	virtual bool checkConfig()
	{
		return true;
	}

	virtual bool run()
	{
		return true;
	}

	virtual bool beforeShut()
	{
		return true;
	}

	virtual bool shut()
	{
		return true;
	}

	virtual bool finalize()
	{
		return true;
	}

	virtual bool onReloadPlugin()
	{
		return true;
	}

	virtual ILibManager* getLibManager() const
	{
		return libManager_;
	}

	std::string strName;

protected:
	ILibManager * libManager_ = NULL;
};

}

