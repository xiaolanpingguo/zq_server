#pragma once


#include "IModule.h"


namespace zq {

class ILogModule : public IModule
{

public:

	enum EN_LOG_LEVEL
	{
		NLL_DEBUG_NORMAL,
		NLL_INFO_NORMAL,
		NLL_WARING_NORMAL,
		NLL_ERROR_NORMAL,
		NLL_FATAL_NORMAL,
	};

	virtual bool logElement(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0) = 0;
	virtual bool logProperty(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0) = 0;
	virtual bool logObject(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strDesc, const char* func = "", int line = 0) = 0;
	virtual bool logRecord(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0) = 0;
	virtual bool logRecord(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0) = 0;

	virtual bool normal(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strInfo, const int64 nDesc, const char* func = "", int line = 0) = 0;
	virtual bool normal(const EN_LOG_LEVEL nll, const uuid ident, const std::string& strInfo, const std::string& strDesc, const  char* func = "", int line = 0) = 0;
	virtual bool normal(const EN_LOG_LEVEL nll, const uuid ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

	virtual bool debug(const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool info(const std::string& strLog, const  char* func = "", int line = 0) = 0;
	virtual bool warning(const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool error(const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool fatal(const std::string& strLog, const char* func = "", int line = 0) = 0;

	virtual bool debug(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool info(const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
	virtual bool warning(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool error(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool fatal(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

	virtual bool debug(const uuid ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool info(const uuid ident, const std::string& strLog, const  char* func = "", int line = 0) = 0;
	virtual bool warning(const uuid ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool error(const uuid ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
	virtual bool fatal(const uuid ident, const std::string& strLog, const char* func = "", int line = 0) = 0;

	virtual bool debug(const uuid ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool info(const uuid ident, const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
	virtual bool warning(const uuid ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool error(const uuid ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
	virtual bool fatal(const uuid ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
};

}
