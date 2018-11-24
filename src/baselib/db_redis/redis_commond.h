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
		vecParam_.emplace_back(std::move(cmd));
	}

	RedisCommand(RedisCommand&& right)
		:vecParam_(std::move(right.vecParam_))
	{
	}

	RedisCommand& operator=(RedisCommand&& right)
	{
		if (this != &right)
		{
			vecParam_ = std::move(right.vecParam_);
		}

		return *this;
	}

	RedisCommand(const RedisCommand& right)
		:vecParam_(right.vecParam_)
	{}

	RedisCommand& operator=(const RedisCommand& right)
	{
		if (this != &right)
		{
			vecParam_ = right.vecParam_;
		}

		return *this;
	}

	~RedisCommand() = default;

	template <typename T>
	RedisCommand& operator<<(const T& t)
	{
		std::stringstream str;
		str << t;
		vecParam_.emplace_back(std::move(str.str()));
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

