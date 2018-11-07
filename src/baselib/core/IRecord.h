#pragma once

#include "data_list.hpp"
#include "baselib/interface_header/platform.h"

namespace zq {

struct RECORD_EVENT_DATA
{
	enum RecordOptype
	{
		add = 0,
		Del,
		Swap,
		Create,
		Update,
		Cleared,
		Sort,
		Cover,

		UNKNOW,
	};
	RECORD_EVENT_DATA()
	{
		nOpType = UNKNOW;
		nRow = 0;
		nCol = 0;
	}

	RecordOptype nOpType;
	int nRow;
	int nCol;
	std::string strRecordName;
};


using RECORD_EVENT_FUNCTOR = std::function<int(const Guid&, const RECORD_EVENT_DATA&, const AbstractData&, const AbstractData&)>;
class ZQ_EXPORT IRecord :public MemoryCounter<IRecord>
{
public:
	IRecord() : MemoryCounter(GET_CLASS_NAME(IRecord))
	{
	}

	typedef std::vector< std::shared_ptr<AbstractData> > TRECORDVEC;
	typedef TRECORDVEC::const_iterator TRECORDVECCONSTITER;

	virtual ~IRecord() {}

	// 该行是否已经在使用(激活)了
	virtual bool isUsed(const int nRow) const = 0;
	virtual bool setUsed(const int nRow, const int bUse) = 0;
	virtual bool preAllocMemoryForRow(const int nRow) = 0;

	// 最大列
	virtual int getCols() const = 0;

	// 最大行
	virtual int getRows() const = 0;

	virtual EN_DATA_TYPE getColType(const int nCol) const = 0;
	virtual const std::string& getColTag(const int nCol) const = 0;


	virtual int addRow(const int nRow) = 0;
	virtual int addRow(const int nRow, const DataList& var) = 0;

	// 以下接口需要在addRow后才可执行成功，可以认为需要每一行需要激活后才可使用
	virtual bool setRow(const int nRow, const DataList& var) = 0;

	virtual bool setInt(const int nRow, const int nCol, const int64 value) = 0;
	virtual bool setFloat(const int nRow, const int nCol, const double value) = 0;
	virtual bool setString(const int nRow, const int nCol, const std::string& value) = 0;
	virtual bool setObject(const int nRow, const int nCol, const Guid& value) = 0;

	virtual bool setInt(const int nRow, const std::string& strColTag, const int64 value) = 0;
	virtual bool setFloat(const int nRow, const std::string& strColTag, const double value) = 0;
	virtual bool setString(const int nRow, const std::string& strColTag, const std::string& value) = 0;
	virtual bool setObject(const int nRow, const std::string& strColTag, const Guid& value) = 0;

	virtual bool queryRow(const int nRow, DataList& varList) = 0;
	virtual bool swapRowInfo(const int nOriginRow, const int nTargetRow) = 0;

	virtual int64 getInt(const int nRow, const int nCol) const = 0;
	virtual double getFloat(const int nRow, const int nCol) const = 0;
	virtual const std::string& getString(const int nRow, const int nCol) const = 0;
	virtual const Guid& getObject(const int nRow, const int nCol) const = 0;

	virtual int64 getInt(const int nRow, const std::string& strColTag) const = 0;
	virtual double getFloat(const int nRow, const std::string& strColTag) const = 0;
	virtual const std::string& getString(const int nRow, const std::string& strColTag) const = 0;
	virtual const Guid& getObject(const int nRow, const std::string& strColTag) const = 0;

	virtual int findRowByColValue(const int nCol, const AbstractData& var, DataList& varResult) = 0;
	virtual int findInt64(const int nCol, const int64 value, DataList& varResult) = 0;
	virtual int findFloat(const int nCol, const double value, DataList& varResult) = 0;
	virtual int findString(const int nCol, const std::string& value, DataList& varResult) = 0;
	virtual int findObject(const int nCol, const Guid& value, DataList& varResult) = 0;

	virtual int findRowByColValue(const int nCol, const AbstractData& var) = 0;
	virtual int findInt64(const int nCol, const int64 value) = 0;
	virtual int findFloat(const int nCol, const double value) = 0;
	virtual int findString(const int nCol, const std::string& valuet) = 0;
	virtual int findObject(const int nCol, const Guid& value) = 0;

	virtual int findRowByColValue(const std::string& strColTag, const AbstractData& var, DataList& varResult) = 0;
	virtual int findInt64(const std::string& strColTag, const int64 value, DataList& varResult) = 0;
	virtual int findFloat(const std::string& strColTag, const double value, DataList& varResult) = 0;
	virtual int findString(const std::string& strColTag, const std::string& value, DataList& varResult) = 0;
	virtual int findObject(const std::string& strColTag, const Guid& value, DataList& varResult) = 0;

	virtual int findRowByColValue(const std::string& strColTag, const AbstractData& var) = 0;
	virtual int findInt64(const std::string& strColTag, const int64 value) = 0;
	virtual int findFloat(const std::string& strColTag, const double value) = 0;
	virtual int findString(const std::string& strColTag, const std::string& value) = 0;
	virtual int findObject(const std::string& strColTag, const Guid& value) = 0;

	virtual bool remove(const int nRow) = 0;
	virtual bool remove(DataList& varRows) //need to optimize
	{
		for (int i = 0; i < varRows.GetCount(); ++i)
		{
			remove((int)varRows.Int(i));
		}

		return true;
	}

	virtual bool clear() = 0;

	virtual void addRecordHook(RECORD_EVENT_FUNCTOR&& cb) = 0;

	virtual const bool getSave() = 0;
	virtual const bool getPublic() = 0;
	virtual const bool getPrivate() = 0;
	virtual const bool getCache() = 0;
	virtual const bool getRef() = 0;
	virtual const bool getForce() = 0;
	virtual const bool getUpload() = 0;
	virtual const std::string& getName() const = 0;

	virtual const std::shared_ptr<DataList> getInitData() const = 0;
	virtual const std::shared_ptr<DataList> getTag() const = 0;

	virtual void setSave(const bool bSave) = 0;
	virtual void setCache(const bool bCache) = 0;
	virtual void setRef(const bool bRef) = 0;
	virtual void setForce(const bool bForce) = 0;
	virtual void setUpload(const bool bUpload) = 0;
	virtual void setPublic(const bool bPublic) = 0;
	virtual void setPrivate(const bool bPrivate) = 0;
	virtual void setName(const std::string& strName) = 0;

	virtual const TRECORDVEC& getRecordVec() const = 0;
};


}
