#pragma once

#include "IRecord.h"
#include "map_ex.hpp"
#include "interface_header/platform.h"

namespace zq {

class ZQ_EXPORT IRecordManager : public MapEx<std::string, IRecord>
{
public:
	virtual ~IRecordManager() {}

	virtual std::shared_ptr<IRecord> AddRecord(const Guid& self, const std::string& strRecordName, const std::shared_ptr<DataList>& TData, const std::shared_ptr<DataList>& tagData, const int nRows) = 0;
	virtual const Guid& self() = 0;

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue) = 0;
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj) = 0;

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value) = 0;
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value) = 0;

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
};


}
