#include "record_manager.h"
using namespace zq;

RecordManager::~RecordManager()
{
    clear();
}

std::shared_ptr<IRecord> RecordManager::AddRecord(const Guid& self, const std::string& strRecordName, const std::shared_ptr<DataList>& ValueList, const std::shared_ptr<DataList>& tagList, const int nRows)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (!pRecord)
    {
        pRecord = std::shared_ptr<IRecord>(new Record(self, strRecordName, ValueList, tagList, nRows));
        this->addElement(strRecordName, pRecord);
    }

    return pRecord;
}

const Guid& RecordManager::self()
{
    return mSelf;
}

bool RecordManager::setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setInt(nRow, nCol, nValue);
    }

    return false;
}

bool RecordManager::setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setInt(nRow, strColTag, value);
    }

    return false;
}

bool RecordManager::setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setFloat(nRow, nCol, dwValue);
    }

    return false;
}

bool RecordManager::setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setFloat(nRow, strColTag, value);
    }

    return false;
}

bool RecordManager::setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setString(nRow, nCol, strValue);
    }

    return false;
}

bool RecordManager::setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setString(nRow, strColTag, value);
    }

    return false;
}

bool RecordManager::setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setObject(nRow, nCol, obj);
    }

    return false;
}

bool RecordManager::setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setObject(nRow, strColTag, value);
    }

    return false;
}

int64 RecordManager::getRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getInt(nRow, nCol);
    }

    return 0;
}

int64 RecordManager::getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getInt(nRow, strColTag);
    }

    return 0;
}


double RecordManager::getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getFloat(nRow, nCol);
    }

    return 0.0;
}

double RecordManager::getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getFloat(nRow, strColTag);
    }

    return 0.0;
}

const std::string& RecordManager::getRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getString(nRow, nCol);
    }

    return NULL_STR;
}

const std::string& RecordManager::getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getString(nRow, strColTag);
    }

    return NULL_STR;
}

const Guid& RecordManager::getRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getObject(nRow, nCol);
    }

    return NULL_OBJECT;
}

const Guid& RecordManager::getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getObject(nRow, strColTag);
    }

    return NULL_OBJECT;
}

