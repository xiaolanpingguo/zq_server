#pragma once

#include "interface_header/base/ILogModule.h"

namespace zq {

class LogModule : public ILogModule
{
public:

	LogModule(ILibManager* p);
	virtual ~LogModule() {}

	virtual bool init();
	virtual bool shut();

	virtual bool beforeShut();
	virtual bool initEnd();

	virtual bool run();

	virtual void logStack();

	virtual bool logElement(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0);
	virtual bool logProperty(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0);
	virtual bool logRecord(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0);
	virtual bool logRecord(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0);
	virtual bool logObject(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strDesc, const char* func = "", int line = 0);

	virtual bool normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const char* func = "", int line = 0);
	virtual bool normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const int64 nDesc, const char* func = "", int line = 0);
	virtual bool normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const std::string& strDesc, const char* func = "", int line = 0);
	virtual bool normal(const EN_LOG_LEVEL nll, const Guid ident, const std::ostringstream& stream, const char* func = "", int line = 0);

	virtual bool debug(const std::string& strLog, const char* func = "", int line = 0);
	virtual bool info(const std::string& strLog, const  char* func = "", int line = 0);
	virtual bool warning(const std::string& strLog, const char* func = "", int line = 0);
	virtual bool error(const std::string& strLog, const char* func = "", int line = 0);
	virtual bool fatal(const std::string& strLog, const char* func = "", int line = 0);

	virtual bool debug(const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool info(const std::ostringstream& stream, const  char* func = "", int line = 0);
	virtual bool warning(const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool error(const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool fatal(const std::ostringstream& stream, const char* func = "", int line = 0);

	virtual bool debug(const Guid ident, const std::string& strLog, const char* func = "", int line = 0);
	virtual bool info(const Guid ident, const std::string& strLog, const  char* func = "", int line = 0);
	virtual bool warning(const Guid ident, const std::string& strLog, const char* func = "", int line = 0);
	virtual bool error(const Guid ident, const std::string& strLog, const char* func = "", int line = 0);
	virtual bool fatal(const Guid ident, const std::string& strLog, const char* func = "", int line = 0);

	virtual bool debug(const Guid ident, const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool info(const Guid ident, const std::ostringstream& stream, const  char* func = "", int line = 0);
	virtual bool warning(const Guid ident, const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool error(const Guid ident, const std::ostringstream& stream, const char* func = "", int line = 0);
	virtual bool fatal(const Guid ident, const std::ostringstream& stream, const char* func = "", int line = 0);

	virtual bool logDebugFunctionDump(const Guid ident, const int nMsg, const std::string& strArg, const char* func = "", const int line = 0);
	virtual bool changeLogLevel(const std::string& strLevel);

protected:
	virtual bool log(const EN_LOG_LEVEL nll, const char* format, ...);

	static bool checkLogFileExist(const char* filename);
	static void rolloutHandler(const char* filename, std::size_t size);

private:
	static unsigned int idx;
	uint64 countTotal_;
};

}

