#include "object.h"
#include "record_manager.h"
#include "property_manager.h"
using namespace zq;

CObject::CObject(Guid self, ILibManager* pLuginManager)
    : IObject(self)
{
	mObjectEventState = COE_CREATE_NODATA;

    mSelf = self;
    m_pPluginManager = pLuginManager;

    recordManager_ = std::shared_ptr<RecordManager>(new RecordManager(mSelf));
    propertyManager_ = std::shared_ptr<PropertyManager>(new PropertyManager(mSelf));
}

CObject::~CObject()
{

}

bool CObject::init()
{
    return true;
}

bool CObject::shut()
{
    return true;
}

bool CObject::run()
{
    return true;
}

bool CObject::addRecordCallBack(const std::string& strRecordName, RECORD_EVENT_FUNCTOR&& cb)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        pRecord->addRecordHook(std::move(cb));

        return true;
    }

    return false;
}

bool CObject::addPropertyCallBack(const std::string& strCriticalName, PROPERTY_EVENT_FUNCTOR&& cb)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strCriticalName);
    if (pProperty)
    {
        pProperty->registerCallback(std::move(cb));
        return true;
    }

    return false;
}

CLASS_OBJECT_EVENT CObject::getState()
{
	return mObjectEventState;
}

bool CObject::setState(const CLASS_OBJECT_EVENT eState)
{
	mObjectEventState = eState;
	return true;
}

bool CObject::findProperty(const std::string& strPropertyName)
{
    if (getPropertyManager()->getElement(strPropertyName))
    {
        return true;
    }

    return false;
}

bool CObject::setPropertyInt(const std::string& strPropertyName, const int64 nValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setInt(nValue);
    }

    return false;
}

bool CObject::setPropertyFloat(const std::string& strPropertyName, const double dwValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setFloat(dwValue);
    }

    return false;
}

bool CObject::setPropertyString(const std::string& strPropertyName, const std::string& strValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setString(strValue);
    }

    return false;
}

bool CObject::setPropertyObject(const std::string& strPropertyName, const Guid& obj)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setObject(obj);
    }

    return false;
}

int64 CObject::getPropertyInt(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getInt();
    }

    return 0;
}

double CObject::getPropertyFloat(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getFloat();
    }

    return 0.0;
}

const std::string& CObject::getPropertyString(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getString();
    }

    return NULL_STR;
}

const Guid& CObject::getPropertyObject(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyManager()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getObject();
    }

    return NULL_OBJECT;
}

bool CObject::findRecord(const std::string& strRecordName)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return true;
    }

    return false;
}

bool CObject::setRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setInt(nRow, nCol, nValue);
    }

    return false;
}

bool CObject::setRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setInt(nRow, strColTag, value);
    }

    return false;
}

bool CObject::setRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setFloat(nRow, nCol, dwValue);
    }

    return false;
}

bool CObject::setRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setFloat(nRow, strColTag, value);
    }

    return false;
}

bool CObject::setRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setString(nRow, nCol, strValue.c_str());
    }

    return false;
}

bool CObject::setRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setString(nRow, strColTag, value.c_str());
    }

    return false;
}

bool CObject::setRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const Guid& obj)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setObject(nRow, nCol, obj);
    }

    return false;
}

bool CObject::setRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->setObject(nRow, strColTag, value);
    }

    return false;
}

int64 CObject::getRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getInt(nRow, nCol);
    }

    return 0;
}

int64 CObject::getRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getInt(nRow, strColTag);
    }

    return 0;
}

double CObject::getRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getFloat(nRow, nCol);
    }

    return 0.0;
}

double CObject::getRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getFloat(nRow, strColTag);
    }

    return 0.0;
}

const std::string& CObject::getRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getString(nRow, nCol);
    }

    return NULL_STR;
}

const std::string& CObject::getRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getString(nRow, strColTag);
    }

    return NULL_STR;
}

const Guid& CObject::getRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getObject(nRow, nCol);
    }

    return NULL_OBJECT;
}

const Guid& CObject::getRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IRecord> pRecord = getRecordManager()->getElement(strRecordName);
    if (pRecord)
    {
        return pRecord->getObject(nRow, strColTag);
    }

    return NULL_OBJECT;
}

std::shared_ptr<IRecordManager> CObject::getRecordManager()
{
    return recordManager_;
}

std::shared_ptr<IPropertyManager> CObject::getPropertyManager()
{
    return propertyManager_;
}

void CObject::setRecordManager(std::shared_ptr<IRecordManager> xRecordManager)
{
	recordManager_ = xRecordManager;
}

void CObject::setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager)
{
	propertyManager_ = xPropertyManager;
}

Guid CObject::self()
{
    return mSelf;
}