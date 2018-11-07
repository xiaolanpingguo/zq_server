#pragma once

#include "lib/interface_header/platform.h"
#include "sql_operation.h"
#include "lib/base_code/producer_consumer_queue.hpp"
#include <atomic>
#include <thread>

namespace zq {


class DatabaseWorker
{
public:
	DatabaseWorker(ProducerConsumerQueue<SQLOperation*>* newQueue, MySQLConnection* connection)
		:connection_(connection),
		queue_(newQueue),
		cancelationToken_(false),
		workerThread_(std::thread(&DatabaseWorker::workerThread, this))
	{

	}

	~DatabaseWorker()
	{
		cancelationToken_ = true;
		queue_->cancel();
		workerThread_.join();
	}

protected:

	void workerThread()
	{
		if (!queue_)
		{
			return;
		}

		for (;;)
		{
			SQLOperation* operation = nullptr;

			queue_->waitAndPop(operation);

			if (cancelationToken_ || !operation)
			{
				return;
			}

			operation->setConnection(connection_);
			operation->call();

			delete operation;
		}
	}

private:
	ProducerConsumerQueue<SQLOperation*>* queue_;
	MySQLConnection* connection_;

	std::thread workerThread_;

	std::atomic<bool> cancelationToken_;

	DatabaseWorker(DatabaseWorker const& right) = delete;
	DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};

}



