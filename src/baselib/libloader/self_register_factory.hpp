#pragma once


#include <map>
#include <string>
#include <functional>
#include <memory>

namespace zq {

class ILib;
class Factory
{

public:

	template<typename T>
	struct register_t
	{
		register_t(const std::string& key)
		{
			Factory::get().map_.emplace(key, [] { return new T(); });
		}

		template<typename... Args>
		register_t(const std::string& key, Args... args)
		{
			Factory::get().map_.emplace(key, [=] { return new T(args...); });
		}
	};

	static ILib* produce(const std::string& key)
	{
		if (map_.find(key) == map_.end())
			throw std::invalid_argument("the key is not exist!");

		return map_[key]();
	}

	static std::unique_ptr<ILib> produce_unique(const std::string& key)
	{
		return std::unique_ptr<ILib>(produce(key));
	}

	static std::shared_ptr<ILib> produce_shared(const std::string& key)
	{
		return std::shared_ptr<ILib>(produce(key));
	}

private:
	Factory() {};
	Factory(const Factory&) = delete;
	Factory(Factory&&) = delete;

	static Factory& get()
	{
		static Factory instance;
		return instance;
	}

	static std::map<std::string, std::function<ILib*()>> map_;
};


std::map<std::string, std::function<ILib*()>> Factory::map_;

#define REGISTER_LIB_VNAME(T) reg_msg_##T##_
#define REGISTER_ILIB(T, key, ...) static Factory::register_t<T> REGISTER_LIB_VNAME(T)(key, ##__VA_ARGS__);

}