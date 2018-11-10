#pragma once

#include "interface_header/base/platform.h"
#include "sql_operation.h"
#include <mutex>
#include <vector>

namespace zq {

class Transaction
{
	friend class TransactionTask;
	friend class MySQLConnection;
	template <typename T> friend class DatabaseWorkerPool;

public:
	Transaction() : cleanedUp_(false) { }
	~Transaction() { cleanup(); }

	void append(PreparedStatement* statement);
	void append(const char* sql);

	std::size_t getSize() const { return vecQueries_.size(); }

protected:
	void cleanup();
	std::vector<SQLElementData> vecQueries_;

private:
	bool cleanedUp_;

};


class TransactionTask : public SQLOperation
{
	template <typename T> friend class DatabaseWorkerPool;
	friend class DatabaseWorker;

public:
	TransactionTask(SQLTransaction trans) : trans_(trans) { }
	~TransactionTask() { }

protected:
	bool execute() override;

	SQLTransaction trans_;
	static std::mutex deadlockLock_;
};

}



