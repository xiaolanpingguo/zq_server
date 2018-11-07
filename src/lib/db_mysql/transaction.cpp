#include "transaction.h"
#include "mysql_connection.h"
#include "prepared_statement.h"
#include <mysqld_error.h>

namespace zq {

void Transaction::append(const char* sql)
{
	SQLElementData data;
	data.type = SQL_ELEMENT_RAW;
	data.element.query = strdup(sql);
	vecQueries_.push_back(data);
}

//- append a prepared statement to the transaction
void Transaction::append(PreparedStatement* stmt)
{
	SQLElementData data;
	data.type = SQL_ELEMENT_PREPARED;
	data.element.stmt = stmt;
	vecQueries_.push_back(data);
}

void Transaction::cleanup()
{
	// This might be called by explicit calls to cleanup or by the auto-destructor
	if (cleanedUp_)
		return;

	for (SQLElementData const &data : vecQueries_)
	{
		switch (data.type)
		{
		case SQL_ELEMENT_PREPARED:
			delete data.element.stmt;
			break;
		case SQL_ELEMENT_RAW:
			free((void*)(data.element.query));
			break;
		}
	}

	vecQueries_.clear();
	cleanedUp_ = true;
}

std::mutex TransactionTask::deadlockLock_;
bool TransactionTask::execute()
{
	int errorCode = conn_->executeTransaction(trans_);
	if (!errorCode)
		return true;

	if (errorCode == ER_LOCK_DEADLOCK)
	{
		// Make sure only 1 async thread retries a transaction so they don't keep dead-locking each other
		std::lock_guard<std::mutex> lock(deadlockLock_);
		uint8 loopBreaker = 5;  // Handle MySQL Errno 1213 without extending deadlock to the core itself
		for (uint8 i = 0; i < loopBreaker; ++i)
		{
			if (!conn_->executeTransaction(trans_))
			{
				return true;
			}
		}
	}

	// Clean up now.
	trans_->cleanup();

	return false;
}

}

