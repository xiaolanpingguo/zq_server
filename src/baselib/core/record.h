#pragma once

#include <vector>
#include "IRecord.h"
#include "data_list.hpp"
#include "map_ex.hpp"
#include "interface_header/platform.h"

namespace zq {

class ZQ_EXPORT Record : public IRecord
{
public:
	Record();
	Record(const Guid& self, const std::string& strRecordName, const std::shared_ptr<DataList>& valueList, const std::shared_ptr<DataList>& tagList, const int nMaxRow);

	virtual ~Record();

	virtual bool isUsed(const int nRow) const;

	virtual bool setUsed(const int nRow, const int bUse);

	virtual bool preAllocMemoryForRow(const int nRow);

	virtual int getCols() const;

	virtual int getRows() const;

	virtual EN_DATA_TYPE getColType(const int nCol) const;
	virtual const std::string& getColTag(const int nCol) const;


	virtual int addRow(const int nRow);

	virtual int addRow(const int nRow, const DataList& var);

	virtual bool setRow(const int nRow, const DataList& var);

	virtual bool setInt(const int nRow, const int nCol, const int64 value);
	virtual bool setFloat(const int nRow, const int nCol, const double value);
	virtual bool setString(const int nRow, const int nCol, const std::string& value);
	virtual bool setObject(const int nRow, const int nCol, const Guid& value);

	virtual bool setInt(const int nRow, const std::string& strColTag, const int64 value);
	virtual bool setFloat(const int nRow, const std::string& strColTag, const double value);
	virtual bool setString(const int nRow, const std::string& strColTag, const std::string& value);
	virtual bool setObject(const int nRow, const std::string& strColTag, const Guid& value);

	virtual bool queryRow(const int nRow, DataList& varList);

	virtual bool swapRowInfo(const int nOriginRow, const int nTargetRow);

	virtual int64 getInt(const int nRow, const int nCol) const;
	virtual double getFloat(const int nRow, const int nCol) const;
	virtual const std::string& getString(const int nRow, const int nCol) const;
	virtual const Guid& getObject(const int nRow, const int nCol) const;

	virtual int64 getInt(const int nRow, const std::string& strColTag) const;
	virtual double getFloat(const int nRow, const std::string& strColTag) const;
	virtual const std::string& getString(const int nRow, const std::string& strColTag) const;
	virtual const Guid& getObject(const int nRow, const std::string& strColTag) const;

	virtual int findRowByColValue(const int nCol, const AbstractData& var, DataList& varResult);
	virtual int findInt64(const int nCol, const int64 value, DataList& varResult);
	virtual int findFloat(const int nCol, const double value, DataList& varResult);
	virtual int findString(const int nCol, const std::string& value, DataList& varResult);
	virtual int findObject(const int nCol, const Guid& value, DataList& varResult);

	virtual int findRowByColValue(const int nCol, const AbstractData& var);
	virtual int findInt64(const int nCol, const int64 value);
	virtual int findFloat(const int nCol, const double value);
	virtual int findString(const int nCol, const std::string& valuet);
	virtual int findObject(const int nCol, const Guid& value);

	virtual int findRowByColValue(const std::string& strColTag, const AbstractData& var, DataList& varResult);
	virtual int findInt64(const std::string& strColTag, const int64 value, DataList& varResult);
	virtual int findFloat(const std::string& strColTag, const double value, DataList& varResult);
	virtual int findString(const std::string& strColTag, const std::string& value, DataList& varResult);
	virtual int findObject(const std::string& strColTag, const Guid& value, DataList& varResult);

	virtual int findRowByColValue(const std::string& strColTag, const AbstractData& var);
	virtual int findInt64(const std::string& strColTag, const int64 value);
	virtual int findFloat(const std::string& strColTag, const double value);
	virtual int findString(const std::string& strColTag, const std::string& value);
	virtual int findObject(const std::string& strColTag, const Guid& value);

	virtual bool remove(const int nRow);

	virtual bool clear();

	virtual void addRecordHook(RECORD_EVENT_FUNCTOR&& cb);

	virtual const bool getSave();

	virtual const bool getCache();

	virtual const bool getRef();

	virtual const bool getForce();

	virtual const bool getUpload();

	virtual const bool getPublic();

	virtual const bool getPrivate();

	virtual const std::string& getName() const;

	virtual void setSave(const bool bSave);

	virtual void setCache(const bool bCache);

	virtual void setRef(const bool bRef);

	virtual void setForce(const bool bForce);

	virtual void setUpload(const bool bUpload);

	virtual void setPublic(const bool bPublic);

	virtual void setPrivate(const bool bPrivate);

	virtual void setName(const std::string& strName);

	// 获得初始化的过后的DataList
	virtual const std::shared_ptr<DataList> getInitData() const;
	virtual const std::shared_ptr<DataList> getTag() const;

	virtual const TRECORDVEC& getRecordVec() const;
protected:
	int getPos(int nRow, int nCol) const;

	int getCol(const std::string& strTag) const;

	bool validPos(int nRow, int nCol) const;
	bool validRow(int nRow) const;
	bool validCol(int nCol) const;

	void onEventHandler(const Guid& self, const RECORD_EVENT_DATA& xEventData, const AbstractData& oldVar, const AbstractData& newVar);

protected:

	std::shared_ptr<DataList> mVarRecordType;
	std::shared_ptr<DataList> mVarRecordTag;

	// tag名字--它的列索引0,1,2...
	std::map<std::string, int> mmTag;

	TRECORDVEC mtRecordVec;
	std::vector<int> mVecUsedState;
	int mnMaxRow;

	Guid mSelf;
	bool mbSave;
	bool mbPublic;
	bool mbPrivate;
	bool mbCache;
	bool mbRef;
	bool mbForce;
	bool mbUpload;
	std::string mstrRecordName;

	using VecEventFunT = std::vector<RECORD_EVENT_FUNCTOR>;
	VecEventFunT recordCb_;
};

}
