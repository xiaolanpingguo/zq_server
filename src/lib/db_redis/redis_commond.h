#pragma once

#include <iostream>
#include <vector>
#include <sstream>

namespace zq {


constexpr const char* REDIS_CRLF = "\r\n"; // Ω· ¯±Í÷æ
class RedisCommand
{
public:
	RedisCommand(const std::string& cmd)
	{
		vecParam_.push_back(cmd);
	}

	~RedisCommand()
	{

	}

	template <typename T>
	RedisCommand& operator<<(const T& t)
	{
		std::stringstream str;
		str << t;
		vecParam_.push_back(str.str());
		return *this;
	}

	std::string serialize() const
	{
		std::stringstream sstream;
		sstream << '*' << vecParam_.size() << REDIS_CRLF;
		for (auto it = vecParam_.begin(); it != vecParam_.end(); ++it)
		{
			sstream << '$' << it->size() << REDIS_CRLF;
			sstream << *it << REDIS_CRLF;
		}

		return sstream.str();
	}

private:

	std::vector<std::string> vecParam_;
};

}

