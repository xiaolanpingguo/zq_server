#include "object.h"
#include "property_manager.h"
using namespace zq;

CObject::CObject(Guid self, ILibManager* pLuginManager)
    : IObject(self)
{
	mObjectEventState = COE_CREATE_NODATA;

    mSelf = self;
    m_pPluginManager = pLuginManager;

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

bool CObject::addPropertyCallBack(const std::string& strCriticalName, PROPERTY_EVENT_FUNCTOR&& cb)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strCriticalName);
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
    if (getPropertyMgr()->getElement(strPropertyName))
    {
        return true;
    }

    return false;
}

bool CObject::setPropertyInt(const std::string& strPropertyName, const int64 nValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setInt(nValue);
    }

    return false;
}

bool CObject::setPropertyDouble(const std::string& strPropertyName, const double dwValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setDouble(dwValue);
    }

    return false;
}

bool CObject::setPropertyString(const std::string& strPropertyName, const std::string& strValue)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setString(strValue);
    }

    return false;
}

bool CObject::setPropertyObject(const std::string& strPropertyName, const Guid& obj)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->setObject(obj);
    }

    return false;
}

int64 CObject::getPropertyInt(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getInt();
    }

    return 0;
}

double CObject::getPropertyDouble(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getDouble();
    }

    return 0.0;
}

const std::string& CObject::getPropertyString(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getString();
    }

    return NULL_STR;
}

const Guid& CObject::getPropertyObject(const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = getPropertyMgr()->getElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->getObject();
    }

    return NULL_OBJECT;
}

std::shared_ptr<IPropertyManager> CObject::getPropertyMgr()
{
    return propertyManager_;
}

void CObject::setPropertyManager(std::shared_ptr<IPropertyManager> xPropertyManager)
{
	propertyManager_ = xPropertyManager;
}

Guid CObject::self()
{
    return mSelf;
}