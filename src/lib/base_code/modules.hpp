#pragma once

#include <initializer_list>
#include <memory>
#include <functional>

namespace zq {

class ModuleBase
{
public:
	virtual ~ModuleBase() {}

public:
	virtual bool init() { return true; }
	virtual bool onInitFromDB() { return true; }
	virtual void onTimer() {}
	virtual void destroy() {}
	virtual int update() { return 0; }
};


template<typename... _TModules>
class ModuleEngine
{
public:

	ModuleEngine()
	{
		modules_ = std::make_tuple(std::shared_ptr<_TModules>(new _TModules())...);
	}

	~ModuleEngine()
	{
	}

	template<typename T>
	std::shared_ptr<T> getModule()
	{
		return std::get<std::shared_ptr<T>>(modules_);
	}

	void initFromDB()
	{
		std::initializer_list<int>
		{
			(std::get<std::shared_ptr<_TModules>>(modules_)->initFromDB(), 0)...
		};
	}

	// 强制降低到1分钟一次
	void timer()
	{
		/*time_t nNow = time(NULL);
		static time_t nLastTime = nNow;
		if (nNow - nLastTime > 60)
		{
			nLastTime = nNow;
			std::initializer_list<int>
			{
				(std::get<std::shared_ptr<_TModules>>(modules_)->onTimer(), 0)...
			};
		}*/
	}
	void destroy()
	{
		std::initializer_list<int>
		{
			(std::get<std::shared_ptr<_TModules>>(modules_)->destroy(), 0)...
		};
	}

public:
	std::tuple<std::shared_ptr<_TModules>...> modules_;
};

}

