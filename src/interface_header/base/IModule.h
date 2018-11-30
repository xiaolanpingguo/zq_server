#pragma once

#include <string>
#include <string_view>
#include "ILibManager.h"
#include "baselib/core/map_ex.hpp"


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

	virtual bool onReload()
	{
		return true;
	}

	virtual bool loadFromDB()
	{
		return true;
	}

	virtual bool saveToDB()
	{
		return true;
	}

	virtual ILibManager* getLibManager() const
	{
		return libManager_;
	}

	std::string strName;

protected:

	ILibManager* libManager_ = nullptr;
};

}

