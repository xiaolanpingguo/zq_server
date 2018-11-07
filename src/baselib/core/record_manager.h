#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include "record.h"
#include "IRecordManager.h"
#include "baselib/interface_header/platform.h"

namespace zq {

class ZQ_EXPORT RecordManager : public IRecordManager
{
public:
	RecordManager(const Guid& self)
	{
		mSelf = self;
	}

	virtual ~RecordManager();

	virtual std::shared_ptr<IRecord> AddRecord(const Guid& self, const std::string& strRecordName, const std::shared_ptr<DataList>& TData, const std::shared_ptr<DataList>& tagData, const int nRows);

	virtual const Guid& self();

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue);
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue);
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue);
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj);

	virtual bool setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value);
	virtual bool setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
	virtual bool setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
	virtual bool setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value);

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol);
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const int nCol);
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const int nCol);

	virtual int64 getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual double getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const std::string& getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
	virtual const Guid& getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);

private:
	Guid mSelf;

};

}

