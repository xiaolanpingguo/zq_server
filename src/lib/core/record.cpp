#include <exception>
#include "data_list.hpp"
#include "record.h"
using namespace zq;

Record::Record()
{
    mSelf = Guid();

    mbSave = false;
    mbPublic = false;
    mbPrivate = false;
    mbCache = false;
	mbRef = false;
	mbForce = false;
	mbUpload = false;

    mstrRecordName = "";
    mnMaxRow = 0;

}

Record::Record(const Guid& self, const std::string& strRecordName, const std::shared_ptr<DataList>& valueList, const std::shared_ptr<DataList>& tagList, const int nMaxRow)
{
	mVarRecordType = valueList;
    mVarRecordTag = tagList;

    mSelf = self;

    mbSave = false;
    mbPublic = false;
    mbPrivate = false;
    mbCache = false;
	mbUpload = false;

    mstrRecordName = strRecordName;

    mnMaxRow = nMaxRow;
    
    mVecUsedState.resize(mnMaxRow);

	// 初始化所有的行都未使用
    for (int i = 0; i < mnMaxRow; i++)
    {
        mVecUsedState[i] = 0;
    }

	// 一个多少个元素，相当于二维数组
    for (int i = 0; i < getRows() * getCols(); i++)
    {
        mtRecordVec.push_back(std::shared_ptr<AbstractData>());
    }

	// 给每一个tag建立索引
    for (int i = 0; i < mVarRecordTag->GetCount(); ++i)
    {
        if (!mVarRecordTag->String(i).empty())
        {
            mmTag[mVarRecordTag->String(i)] = i;
        }
    }
}

Record::~Record()
{
    for (TRECORDVEC::iterator iter = mtRecordVec.begin(); iter != mtRecordVec.end(); ++iter)
    {
        iter->reset();
    }

    mtRecordVec.clear();
    mVecUsedState.clear();
    recordCb_.clear();
}

int Record::getCols() const
{
    return mVarRecordType->GetCount();
}

int Record::getRows() const
{
    return mnMaxRow;
}

EN_DATA_TYPE Record::getColType(const int nCol) const
{
    return mVarRecordType->Type(nCol);
}

const std::string& Record::getColTag(const int nCol) const
{
    return mVarRecordTag->String(nCol);
}


int Record::addRow(const int nRow)
{
    return addRow(nRow, *mVarRecordType);
}

int Record::addRow(const int nRow, const DataList& var)
{
	bool bCover = false;
    int nFindRow = nRow;
    if (nFindRow >= mnMaxRow)
    {
        return -1;
    }

    if (var.GetCount() != getCols())
    {
        return -1;
    }

    if (nFindRow < 0)
    {
        for (int i = 0; i < mnMaxRow; i++)
        {
            if (!isUsed(i))
            {
                nFindRow = i;
                break;
            }
        }
    }
	else
	{
		if (isUsed(nFindRow))
	    {
	        bCover = true;
	    }		
	}

    if (nFindRow < 0)
    {
        return -1;
    }

    for (int i = 0; i < getCols(); ++i)
    {
        if (var.Type(i) != getColType(i))
        {
            return -1;
        }
    }

    setUsed(nFindRow, 1);

	// 赋值
    for (int i = 0; i < getCols(); ++i)
    {
        std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nFindRow, i));
		if (nullptr == pVar)
		{
			pVar = std::shared_ptr<AbstractData>(new AbstractData(var.Type(i)));
		}

		pVar->variantData = var.GetStack(i)->variantData;
    }

	RECORD_EVENT_DATA xEventData;
	xEventData.nOpType = bCover? RECORD_EVENT_DATA::Cover : RECORD_EVENT_DATA::add;
	xEventData.nRow = nFindRow;
	xEventData.nCol = 0;
	xEventData.strRecordName = mstrRecordName;

	AbstractData tData;
    onEventHandler(mSelf, xEventData, tData, tData); //FIXME:RECORD

    return nFindRow;
}

bool Record::setRow(const int nRow, const DataList & var)
{
	if (var.GetCount() != getCols())
	{
		return false;
	}

	if (!isUsed(nRow))
	{
		return false;
	}

	for (int i = 0; i < getCols(); ++i)
	{
		if (var.Type(i) != getColType(i))
		{
			return false;
		}
	}

	for (int i = 0; i < getCols(); ++i)
	{
		std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, i));
		if (nullptr == pVar)
		{
			pVar = std::shared_ptr<AbstractData>(new AbstractData(var.Type(i)));
		}

		AbstractData oldValue = *pVar;

		pVar->variantData = var.GetStack(i)->variantData;

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = i;
		xEventData.strRecordName = mstrRecordName;

		onEventHandler(mSelf, xEventData, oldValue, *pVar);
	}

	return false;
}

bool Record::setInt(const int nRow, const int nCol, const int64 value)
{
    if (!validPos(nRow, nCol))
    {
        return false;
    }

    if (TDATA_INT != getColType(nCol))
    {
        return false;
    }

    if (!isUsed(nRow))
    {
        return false;
    }

	AbstractData var;
    var.setInt(value);

    std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));
	//must have memory
	if (nullptr == pVar)
	{
		return false;
	}

	if (var == *pVar)
	{
		return false;
	}

	if (recordCb_.size() == 0)
	{
		pVar->setInt(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue.setInt(pVar->getInt());

		pVar->setInt(value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = mstrRecordName;

		onEventHandler(mSelf, xEventData, oldValue, *pVar);
	}

    return true;
}

bool Record::setInt(const int nRow, const std::string& strColTag, const int64 value)
{
    int nCol = getCol(strColTag);
    return setInt(nRow, nCol, value);
}

bool Record::setFloat(const int nRow, const int nCol, const double value)
{
    if (!validPos(nRow, nCol))
    {
        return false;
    }

    if (TDATA_FLOAT != getColType(nCol))
    {
        return false;
    }

    if (!isUsed(nRow))
    {
        return false;
    }

    AbstractData var;
    var.setFloat(value);

    std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));

	//must have memory
	if (nullptr == pVar)
	{
		return false;
	}

	if (var == *pVar)
	{
		return false;
	}

	if (recordCb_.size() == 0)
	{
		pVar->setFloat(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue.setFloat(pVar->getFloat());
		pVar->setFloat(value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = mstrRecordName;

		onEventHandler(mSelf, xEventData, oldValue, *pVar);
	}

    return true;
}

bool Record::setFloat(const int nRow, const std::string& strColTag, const double value)
{
    int nCol = getCol(strColTag);
    return setFloat(nRow, nCol, value);
}

bool Record::setString(const int nRow, const int nCol, const std::string& value)
{
    if (!validPos(nRow, nCol))
    {
        return false;
    }

    if (TDATA_STRING != getColType(nCol))
    {
        return false;
    }

    if (!isUsed(nRow))
    {
        return false;
    }

    AbstractData var;
    var.setString(value);

    std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));

	//must have memory
	if (nullptr == pVar)
	{
		return false;
	}

	if (var == *pVar)
	{
		return false;
	}

	if (recordCb_.size() == 0)
	{
		pVar->setString(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue.setString(pVar->getString());

		pVar->setString(value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = mstrRecordName;

		onEventHandler(mSelf, xEventData, oldValue, *pVar);
	}

    return true;
}

bool Record::setString(const int nRow, const std::string& strColTag, const std::string& value)
{
    int nCol = getCol(strColTag);
    return setString(nRow, nCol, value);
}

bool Record::setObject(const int nRow, const int nCol, const Guid& value)
{
    if (!validPos(nRow, nCol))
    {
        return false;
    }

    if (TDATA_OBJECT != getColType(nCol))
    {
        return false;
    }

    if (!isUsed(nRow))
    {
        return false;
    }

    AbstractData var;
    var.setObject(value);

    std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));

	//must have memory
	if (nullptr == pVar)
	{
		return false;
	}

	if (var == *pVar)
	{
		return false;
	}

	if (recordCb_.size() == 0)
	{
		pVar->setObject(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue.setObject(pVar->getObject());

		pVar->setObject(value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = mstrRecordName;

		onEventHandler(mSelf, xEventData, oldValue, *pVar);
	}

    return true;
}

bool Record::setObject(const int nRow, const std::string& strColTag, const Guid& value)
{
	int nCol = getCol(strColTag);
	return setObject(nRow, nCol, value);
}

bool Record::queryRow(const int nRow, DataList& varList)
{
    if (!validRow(nRow))
    {
        return false;
    }

    if (!isUsed(nRow))
    {
        return false;
    }

    varList.clear();
    for (int i = 0; i < getCols(); ++i)
    {
        std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, i));
        if (pVar)
        {
            varList.Append(*pVar);
        }
        else
        {
            switch (getColType(i))
            {
                case TDATA_INT:
                    varList.add(int64(0));
                    break;

                case TDATA_FLOAT:
                    varList.add(0.0f);
                    break;

                case TDATA_STRING:
                    varList.add(NULL_STR.c_str());
                    break;

                case TDATA_OBJECT:
                    varList.add(Guid());
                    break;
                default:
                    return false;
                    break;
            }
        }
    }

    if (varList.GetCount() != getCols())
    {
        return false;
    }

    return true;
}

int64 Record::getInt(const int nRow, const int nCol) const
{
    if (!validPos(nRow, nCol))
    {
        return 0;
    }

    if (!isUsed(nRow))
    {
        return 0;
    }

    const std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));
    if (!pVar)
    {
        return 0;
    }

    return pVar->getInt();
}

int64 Record::getInt(const int nRow, const std::string& strColTag) const
{
    int nCol = getCol(strColTag);
    return getInt(nRow, nCol);
}

double Record::getFloat(const int nRow, const int nCol) const
{
    if (!validPos(nRow, nCol))
    {
        return 0.0f;
    }

    if (!isUsed(nRow))
    {
        return 0.0f;
    }

    const std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));
    if (!pVar)
    {
        return 0.0f;
    }

    return pVar->getFloat();
}

double Record::getFloat(const int nRow, const std::string& strColTag) const
{
    int nCol = getCol(strColTag);
    return getFloat(nRow, nCol);
}

const std::string& Record::getString(const int nRow, const int nCol) const
{
    if (!validPos(nRow, nCol))
    {
        return NULL_STR;
    }

    if (!isUsed(nRow))
    {
        return NULL_STR;
    }

    const std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));
    if (!pVar)
    {
        return NULL_STR;
    }

    return pVar->getString();
}

const std::string& Record::getString(const int nRow, const std::string& strColTag) const
{
    int nCol = getCol(strColTag);
    return getString(nRow, nCol);
}

const Guid& Record::getObject(const int nRow, const int nCol) const
{
    if (!validPos(nRow, nCol))
    {
        return NULL_OBJECT;
    }

    if (!isUsed(nRow))
    {
        return NULL_OBJECT;
    }

    const  std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, nCol));
    if (!pVar)
    {
        return NULL_OBJECT;
    }

    return pVar->getObject();
}

const Guid& Record::getObject(const int nRow, const std::string& strColTag) const
{
    int nCol = getCol(strColTag);
    return getObject(nRow, nCol);
}

int Record::findRowByColValue(const int nCol, const AbstractData& var, DataList& varResult)
{
    if (!validCol(nCol))
    {
        return -1;
    }

    EN_DATA_TYPE eType = var.getType();
    if (eType != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    switch (eType)
    {
        case TDATA_INT:
            return findInt64(nCol, var.getInt(), varResult);
            break;

        case TDATA_FLOAT:
            return findFloat(nCol, var.getFloat(), varResult);
            break;

        case TDATA_STRING:
            return findString(nCol, var.getString(), varResult);
            break;

        case TDATA_OBJECT:
            return findObject(nCol, var.getObject(), varResult);
            break;

        default:
            break;
    }

    return -1;
}

int Record::findRowByColValue(const std::string& strColTag, const AbstractData& var, DataList& varResult)
{
    int nCol = getCol(strColTag);
    return findRowByColValue(nCol, var, varResult);
}

int Record::findInt64(const int nCol, const int64 value, DataList& varResult)
{
    if (!validCol(nCol))
    {
        return -1;
    }

    if (TDATA_INT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
        for (int i = 0; i < mnMaxRow; ++i)
        {
            if (!isUsed(i))
            {
                continue;
            }

            if (getInt(i, nCol) == value)
            {
                varResult << i;
            }
        }

        return varResult.GetCount();
    }

    return -1;
}

int Record::findInt64(const std::string& strColTag, const int64 value, DataList& varResult)
{
    if (strColTag.empty())
    {
        return -1;
    }

    int nCol = getCol(strColTag);
    return findInt64(nCol, value, varResult);
}

int Record::findFloat(const int nCol, const double value, DataList& varResult)
{
    if (!validCol(nCol))
    {
        return -1;
    }

    if (TDATA_FLOAT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    for (int i = 0; i < mnMaxRow; ++i)
    {
        if (!isUsed(i))
        {
            continue;
        }

        if (getFloat(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int Record::findFloat(const std::string& strColTag, const double value, DataList& varResult)
{
    if (strColTag.empty())
    {
        return -1;
    }

    int nCol = getCol(strColTag);
    return findFloat(nCol, value, varResult);
}

int Record::findString(const int nCol, const std::string& value, DataList& varResult)
{
    if (!validCol(nCol))
    {
        return -1;
    }

    if (TDATA_STRING != mVarRecordType->Type(nCol))
    {
        return -1;
    }


    {
        for (int i = 0; i < mnMaxRow; ++i)
        {
            if (!isUsed(i))
            {
                continue;
            }

            const std::string& strData = getString(i, nCol);
            if (0 == strcmp(strData.c_str(), value.c_str()))
            {
                varResult << (int64)i;
            }
        }

        return varResult.GetCount();
    }

    return -1;
}

int Record::findString(const std::string& strColTag, const std::string& value, DataList& varResult)
{
    if (strColTag.empty())
    {
        return -1;
    }

    int nCol = getCol(strColTag);
    return findString(nCol, value, varResult);
}

int Record::findObject(const int nCol, const Guid& value, DataList& varResult)
{
    if (!validCol(nCol))
    {
        return -1;
    }

    if (TDATA_OBJECT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
        for (int i = 0; i < mnMaxRow; ++i)
        {
            if (!isUsed(i))
            {
                continue;
            }

            if (getObject(i, nCol) == value)
            {
                varResult << (int64)i;
            }
        }

        return varResult.GetCount();
    }

    return -1;
}

int Record::findObject(const std::string& strColTag, const Guid& value, DataList& varResult)
{
    if (strColTag.empty())
    {
        return -1;
    }

    int nCol = getCol(strColTag);
    return findObject(nCol, value, varResult);
}

int Record::findRowByColValue(const int nCol, const AbstractData & var)
{
	DataList xDataList;
	int nRowCount = findRowByColValue(nCol, var, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findInt64(const int nCol, const int64 value)
{
	DataList xDataList;
	int nRowCount = findInt64(nCol, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findFloat(const int nCol, const double value)
{
	DataList xDataList;
	int nRowCount = findFloat(nCol, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findString(const int nCol, const std::string & value)
{
	DataList xDataList;
	int nRowCount = findString(nCol, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findObject(const int nCol, const Guid & value)
{
	DataList xDataList;
	int nRowCount = findObject(nCol, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findRowByColValue(const std::string & strColTag, const AbstractData & var)
{
	DataList xDataList;
	int nRowCount = findRowByColValue(strColTag, var, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findInt64(const std::string & strColTag, const int64 value)
{
	DataList xDataList;
	int nRowCount = findInt64(strColTag, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findFloat(const std::string & strColTag, const double value)
{
	DataList xDataList;
	int nRowCount = findFloat(strColTag, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findString(const std::string & strColTag, const std::string & value)
{
	DataList xDataList;
	int nRowCount = findString(strColTag, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

int Record::findObject(const std::string & strColTag, const Guid & value)
{
	DataList xDataList;
	int nRowCount = findObject(strColTag, value, xDataList);
	if (nRowCount > 0 && xDataList.GetCount() > 0)
	{
		return (int) xDataList.Int(0);
	}

	return -1;
}

bool Record::remove(const int nRow)
{
    if (validRow(nRow))
    {
        if (isUsed(nRow))
        {
			RECORD_EVENT_DATA xEventData;
			xEventData.nOpType = RECORD_EVENT_DATA::Del;
			xEventData.nRow = nRow;
			xEventData.nCol = 0;
			xEventData.strRecordName = mstrRecordName;

			onEventHandler(mSelf, xEventData, AbstractData(), AbstractData());

			mVecUsedState[nRow] = 0;

			return true;
        }
    }

    return false;
}

bool Record::clear()
{
    for (int i = getRows() - 1; i >= 0; i--)
    {
        remove(i);
    }

    return true;
}

void Record::addRecordHook(RECORD_EVENT_FUNCTOR&& cb)
{
    recordCb_.emplace_back(std::move(cb));
}

const bool Record::getSave()
{
    return mbSave;
}

const bool Record::getCache()
{
    return mbCache;
}

const bool Record::getRef()
{
	return mbRef;
}

const bool Record::getForce()
{
	return mbForce;
}

const bool Record::getUpload()
{
	return mbUpload;
}

const bool Record::getPublic()
{
    return mbPublic;
}

const bool Record::getPrivate()
{
    return mbPrivate;
}

int Record::getPos(int nRow, int nCol) const
{
    return nRow * mVarRecordType->GetCount() + nCol;
}

const std::string& Record::getName() const
{
    return mstrRecordName;
}

void Record::setSave(const bool bSave)
{
    mbSave = bSave;
}

void Record::setCache(const bool bCache)
{
    mbCache = bCache;
}

void Record::setRef(const bool bRef)
{
	mbRef = bRef;
}

void Record::setForce(const bool bForce)
{
	mbForce = bForce;
}

void Record::setUpload(const bool bUpload)
{
	mbUpload = bUpload;
}

void Record::setPublic(const bool bPublic)
{
    mbPublic = bPublic;
}

void Record::setPrivate(const bool bPrivate)
{
    mbPrivate = bPrivate;
}

void Record::setName(const std::string& strName)
{
    mstrRecordName = strName;
}

const std::shared_ptr<DataList> Record::getInitData() const
{
    std::shared_ptr<DataList> pIniData = std::shared_ptr<DataList>(new DataList());
    pIniData->Append(*mVarRecordType);

    return pIniData;
}

void Record::onEventHandler(const Guid& self, const RECORD_EVENT_DATA& xEventData, const AbstractData& oldVar, const AbstractData& newVar)
{
    for (auto it = recordCb_.begin(); it != recordCb_.end(); ++it)
    {      
		(*it)(self, xEventData, oldVar, newVar);
    }
}

bool Record::isUsed(const int nRow) const
{
    if (validRow(nRow))
    {
        return (mVecUsedState[nRow] > 0);
    }

    return false;
}

bool Record::swapRowInfo(const int nOriginRow, const int nTargetRow)
{
    if (!isUsed(nOriginRow))
    {
        return false;
    }

    if (validRow(nOriginRow) && validRow(nTargetRow))
    {
        for (int i = 0; i < getCols(); ++i)
        {
            std::shared_ptr<AbstractData> pOrigin = mtRecordVec.at(getPos(nOriginRow, i));
            mtRecordVec[getPos(nOriginRow, i)] = mtRecordVec.at(getPos(nTargetRow, i));
            mtRecordVec[getPos(nTargetRow, i)] = pOrigin;
        }

        int nOriginUse = mVecUsedState[nOriginRow];
        mVecUsedState[nOriginRow] = mVecUsedState[nTargetRow];
        mVecUsedState[nTargetRow] = nOriginUse;

        RECORD_EVENT_DATA xEventData;
        xEventData.nOpType = RECORD_EVENT_DATA::Swap;
        xEventData.nRow = nOriginRow;
        xEventData.nCol = nTargetRow;
        xEventData.strRecordName = mstrRecordName;

        AbstractData xData;
        onEventHandler(mSelf, xEventData, xData, xData);

        return true;
    }

    return false;
}

const std::shared_ptr<DataList> Record::getTag() const
{
    std::shared_ptr<DataList> pIniData = std::shared_ptr<DataList>(new DataList());
    pIniData->Append(*mVarRecordTag);
    return pIniData;
}

const IRecord::TRECORDVEC& Record::getRecordVec() const
{
    return mtRecordVec;
}

bool Record::setUsed(const int nRow, const int bUse)
{
    if (validRow(nRow))
    {
        mVecUsedState[nRow] = bUse;
        return true;
    }

    return false;
}

bool Record::preAllocMemoryForRow(const int nRow)
{
	if (!isUsed(nRow))
	{
		return false;
	}

	for (int i = 0; i < getCols(); ++i)
	{
		std::shared_ptr<AbstractData>& pVar = mtRecordVec.at(getPos(nRow, i));
		if (nullptr == pVar)
		{
			pVar = std::shared_ptr<AbstractData>(new AbstractData(mVarRecordType->Type(i)));
		}

		pVar->variantData = mVarRecordType->GetStack(i)->variantData;
	}
	return true;
}

bool Record::validPos(int nRow, int nCol) const
{
    if (validCol(nCol) && validRow(nRow))
    {
        return true;
    }

    return false;
}

bool Record::validRow(int nRow) const
{
    if (nRow >= getRows() || nRow < 0)
    {
        return false;
    }

    return true;
}

bool Record::validCol(int nCol) const
{
    if (nCol >= getCols() || nCol < 0)
    {
        return false;
    }

    return true;
}

int Record::getCol(const std::string& strTag) const
{
    auto it = mmTag.find(strTag);
    if (it != mmTag.end())
    {
        return it->second;
    }

    return -1;
}
