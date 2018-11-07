//#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <stdarg.h>
#include "log_module.h"
#include "termcolor.hpp"
#include "easylogging++.h"	 // 这里包含了windows.h，所以要在后面包含，不然会和winsock冲突

INITIALIZE_EASYLOGGINGPP

namespace zq {


constexpr const char* LOG_CONFIG_DIR_NAME = "logconfig/";
constexpr const char* LOG_DIR= "log/";

unsigned int LogModule::idx = 0;

bool LogModule::checkLogFileExist(const char* filename)
{
	std::stringstream stream;
	stream << filename << "." << ++idx;
	std::fstream file;
	file.open(stream.str(), std::ios::in);
	if (file)
	{
		return checkLogFileExist(filename);
	}

	return false;
}

void LogModule::rolloutHandler(const char* filename, std::size_t size)
{
	std::stringstream stream;
	if (!checkLogFileExist(filename))
	{
		stream << filename << "." << idx;
		rename(filename, stream.str().c_str());
	}
}

LogModule::LogModule(ILibManager* p)
{
	libManager_ = p;
}

bool LogModule::init()
{
	countTotal_ = 0;

	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

	std::string strAppLogName;
	std::string server_name = libManager_->getServerName();
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	strAppLogName = LOG_CONFIG_DIR_NAME + server_name + "_win.conf";
#else
	strAppLogName = LOG_CONFIG_DIR_NAME + server_name + ".conf";
#endif

	el::Configurations conf(strAppLogName);

	el::Configuration* pConfiguration = conf.get(el::Level::Debug, el::ConfigurationType::Filename);
	const std::string& strFileName = pConfiguration->value();
	pConfiguration->setValue(LOG_DIR + strFileName);

	std::cout << "LogConfig: " << strAppLogName << std::endl;

	el::Loggers::reconfigureAllLoggers(conf);
	el::Helpers::installPreRollOutCallback(rolloutHandler);

	return true;
}

bool LogModule::shut()
{
	el::Helpers::uninstallPreRollOutCallback();

	return true;
}

bool LogModule::beforeShut()
{
	return true;

}

bool LogModule::initEnd()
{

	return true;
}

bool LogModule::run()
{
	return true;

}

bool LogModule::log(const EN_LOG_LEVEL nll, const char* format, ...)
{
	countTotal_++;

	char szBuffer[1024 * 10] = { 0 };

	va_list args;
	va_start(args, format);
	vsnprintf(szBuffer, sizeof(szBuffer) - 1, format, args);
	va_end(args);

	switch (nll)
	{
	case ILogModule::NLL_DEBUG_NORMAL:
	{
		std::cout << termcolor::green;
		LOG(DEBUG) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
	}
	break;
	case ILogModule::NLL_INFO_NORMAL:
	{
		std::cout << termcolor::green;
		LOG(INFO) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
	}
	break;
	case ILogModule::NLL_WARING_NORMAL:
	{
		std::cout << termcolor::yellow;
		LOG(WARNING) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
	}
	break;
	case ILogModule::NLL_ERROR_NORMAL:
	{
		std::cout << termcolor::red;
		LOG(ERROR) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
		//logStack();
	}
	break;
	case ILogModule::NLL_FATAL_NORMAL:
	{
		std::cout << termcolor::red;
		LOG(FATAL) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
	}
	break;
	default:
	{
		std::cout << termcolor::green;
		LOG(INFO) << countTotal_ << " | " << libManager_->getServerID() << " | " << szBuffer;
	}
	break;
	}

	std::cout << termcolor::reset;

	return true;
}

bool LogModule::logElement(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strElement, const std::string& strDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "[ELEMENT] Indent[%s] Element[%s] %s %s %d", ident.toString().c_str(), strElement.c_str(), strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "[ELEMENT] Indent[%s] Element[%s] %s", ident.toString().c_str(), strElement.c_str(), strDesc.c_str());
	}

	return true;
}

bool LogModule::logProperty(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strProperty, const std::string& strDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "[PROPERTY] Indent[%s] Property[%s] %s %s %d", ident.toString().c_str(), strProperty.c_str(), strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "[PROPERTY] Indent[%s] Property[%s] %s", ident.toString().c_str(), strProperty.c_str(), strDesc.c_str());
	}

	return true;

}

bool LogModule::logRecord(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s %s %d", ident.toString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s", ident.toString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str());
	}

	return true;

}

bool LogModule::logRecord(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strRecord, const std::string& strDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "[RECORD] Indent[%s] Record[%s] %s %s %d", ident.toString().c_str(), strRecord.c_str(), strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "[RECORD] Indent[%s] Record[%s] %s", ident.toString().c_str(), strRecord.c_str(), strDesc.c_str());
	}

	return true;
}

bool LogModule::logObject(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "[OBJECT] Indent[%s] %s %s %d", ident.toString().c_str(), strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "[OBJECT] Indent[%s] %s", ident.toString().c_str(), strDesc.c_str());
	}

	return true;

}

void LogModule::logStack()
{

	//To add
	/*
	#ifdef ZQ_DEBUG_MODE
	time_t t = time(0);
	char szDmupName[MAX_PATH];
	tm* ptm = localtime(&t);

	sprintf(szDmupName, "%d_%d_%d_%d_%d_%d.dmp",  ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	HANDLE hDumpFile = CreateFile(szDmupName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	//dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;


	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);

	#endif
	*/
}

bool LogModule::normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const std::string& strDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "Indent[%s] %s %s %s %d", ident.toString().c_str(), strInfo.c_str(), strDesc.c_str(), func, line);
	}
	else
	{
		log(nll, "Indent[%s] %s %s", ident.toString().c_str(), strInfo.c_str(), strDesc.c_str());
	}

	return true;
}

bool LogModule::normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const int64 nDesc, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "Indent[%s] %s %lld %s %d", ident.toString().c_str(), strInfo.c_str(), nDesc, func, line);
	}
	else
	{
		log(nll, "Indent[%s] %s %lld", ident.toString().c_str(), strInfo.c_str(), nDesc);
	}

	return true;
}

bool LogModule::normal(const EN_LOG_LEVEL nll, const Guid ident, const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(nll, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(nll, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

bool LogModule::normal(const EN_LOG_LEVEL nll, const Guid ident, const std::string& strInfo, const char* func /*= ""*/, int line /*= 0*/)
{
	if (line > 0)
	{
		log(nll, "Indent[%s] %s %s %d", ident.toString().c_str(), strInfo.c_str(), func, line);
	}
	else
	{
		log(nll, "Indent[%s] %s", ident.toString().c_str(), strInfo.c_str());
	}

	return true;
}

bool LogModule::logDebugFunctionDump(const Guid ident, const int nMsg, const std::string& strArg, const char* func /*= ""*/, const int line /*= 0*/)
{
	//#ifdef ZQ_DEBUG_MODE
	normal(ILogModule::NLL_WARING_NORMAL, ident, strArg + "MsgID:", nMsg, func, line);
	//#endif
	return true;
}

bool LogModule::changeLogLevel(const std::string& strLevel)
{
	el::Level logLevel = el::LevelHelper::convertFromString(strLevel.c_str());
	el::Logger* pLogger = el::Loggers::getLogger("default");
	if (NULL == pLogger)
	{
		return false;
	}

	el::Configurations* pConfigurations = pLogger->configurations();
	if (NULL == pConfigurations)
	{
		return false;
	}

	switch (logLevel)
	{
	case el::Level::Fatal:
	{
		el::Configuration errorConfiguration(el::Level::Error, el::ConfigurationType::Enabled, "false");
		pConfigurations->set(&errorConfiguration);
	}
	case el::Level::Error:
	{
		el::Configuration warnConfiguration(el::Level::Warning, el::ConfigurationType::Enabled, "false");
		pConfigurations->set(&warnConfiguration);
	}
	case el::Level::Warning:
	{
		el::Configuration infoConfiguration(el::Level::Info, el::ConfigurationType::Enabled, "false");
		pConfigurations->set(&infoConfiguration);
	}
	case el::Level::Info:
	{
		el::Configuration debugConfiguration(el::Level::Debug, el::ConfigurationType::Enabled, "false");
		pConfigurations->set(&debugConfiguration);

	}
	case el::Level::Debug:
		break;
	default:
		break;
	}

	el::Loggers::reconfigureAllLoggers(*pConfigurations);
	normal(ILogModule::NLL_INFO_NORMAL, Guid(), "[log] Change log level", strLevel, __FUNCTION__, __LINE__);
	return true;
}

bool LogModule::debug(const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "%s %s %d", strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "%s", strLog.c_str());
	}

	return true;
}

bool LogModule::info(const std::string& strLog, const  char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "%s %s %d", strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "%s", strLog.c_str());
	}

	return true;
}

bool LogModule::warning(const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "%s %s %d", strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "%s", strLog.c_str());
	}

	return true;
}

bool LogModule::error(const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "%s %s %d", strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "%s", strLog.c_str());
	}

	return true;
}

bool LogModule::fatal(const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "%s %s %d", strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "%s", strLog.c_str());
	}

	return true;
}


bool LogModule::debug(const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "%s", stream.str().c_str());
	}

	return true;
}

bool LogModule::info(const std::ostringstream& stream, const  char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "%s", stream.str().c_str());
	}

	return true;
}

bool LogModule::warning(const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "%s", stream.str().c_str());
	}

	return true;
}

bool LogModule::error(const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "%s", stream.str().c_str());
	}

	return true;
}

bool LogModule::fatal(const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "%s", stream.str().c_str());
	}

	return true;
}


bool LogModule::debug(const Guid ident, const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s", ident.toString().c_str(), strLog.c_str());
	}

	return true;
}

bool LogModule::info(const Guid ident, const std::string& strLog, const  char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s", ident.toString().c_str(), strLog.c_str());
	}

	return true;
}

bool LogModule::warning(const Guid ident, const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s", ident.toString().c_str(), strLog.c_str());
	}

	return true;
}

bool LogModule::error(const Guid ident, const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s", ident.toString().c_str(), strLog.c_str());
	}

	return true;
}

bool LogModule::fatal(const Guid ident, const std::string& strLog, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), strLog.c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s", ident.toString().c_str(), strLog.c_str());
	}

	return true;
}


bool LogModule::debug(const Guid ident, const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

bool LogModule::info(const Guid ident, const std::ostringstream& stream, const  char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

bool LogModule::warning(const Guid ident, const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

bool LogModule::error(const Guid ident, const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

bool LogModule::fatal(const Guid ident, const std::ostringstream& stream, const char* func, int line)
{
	if (line > 0)
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s %s %d", ident.toString().c_str(), stream.str().c_str(), func, line);
	}
	else
	{
		log(EN_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s", ident.toString().c_str(), stream.str().c_str());
	}

	return true;
}

}
