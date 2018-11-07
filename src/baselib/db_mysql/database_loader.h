#pragma once


#include "interface_header/platform.h"
#include "database_worker_pool.h"
#include <functional>
#include <queue>
#include <stack>
#include <string>

namespace zq {

template<typename T> class DatabaseWorkerPool;

class DatabaseLoader
{
	using Predicate = std::function<bool()>;
	using Closer = std::function<void()>;

public:
	DatabaseLoader() = default;
	~DatabaseLoader() = default;

	// 添加一个要连接的table
	template<typename T>
	DatabaseLoader& addDatabase(DatabaseWorkerPool<T>& pool, std::string const& conn_str, int asyncThreads = 1, int synchThreads = 1)
	{
		open_.push([this, conn_str, &pool, asyncThreads, synchThreads]() -> bool
		{
			if (conn_str.empty())
			{
				LOG_ERROR << "name is empty";
				return false;
			}

			pool.setConnectionInfo(conn_str, asyncThreads, synchThreads);
			uint32 error = pool.open();
			if (error != 0)
			{
				LOG_ERROR << "open db failed, error: " << error << " ,conn_str: " << conn_str;
				return false;
			}

			close_.push([&pool]
			{
				pool.close();
			});
			return true;
		});

		prepare_.push([this, conn_str, &pool]() -> bool
		{
			if (!pool.prepareStatements())
			{
				LOG_ERROR << "prepare stament error, conn_str: " << conn_str;
				return false;
			}
			return true;
		});

		return *this;
	}

	// 加载
	bool load()
	{
		if (!openDatabases())
			return false;

		if (!populateDatabases())
			return false;

		if (!updateDatabases())
			return false;

		if (!prepareStatements())
			return false;

		return true;
	}

private:
	bool openDatabases()
	{
		return process(open_);
	}

	bool populateDatabases()
	{
		return process(populate_);
	}

	bool updateDatabases()
	{
		return process(update_);
	}

	bool prepareStatements()
	{
		return process(prepare_);
	}

	bool process(std::queue<Predicate>& queue)
	{
		while (!queue.empty())
		{
			if (!queue.front()())
			{
				// 如果发生错误，就close 
				while (!close_.empty())
				{
					close_.top()();
					close_.pop();
				}

				return false;
			}

			queue.pop();
		}
		return true;
	}

private:

	const bool autoSetup_ = true;
	const uint32 updateFlags_ = 0;

	std::queue<Predicate> open_, populate_, update_, prepare_;
	std::stack<Closer> close_;
};

}



