//#include "property.h"
//#include "property_manager.h"
//using namespace zq;
//
//PropertyManager::~PropertyManager()
//{
//	clear();
//}
//
//bool PropertyManager::registerCallback(const std::string& strProperty, PropertyEventFunT&& cb)
//{
//	std::shared_ptr<IProperty> pProperty = this->getElement(strProperty);
//	if (pProperty)
//	{
//		pProperty->registerCallback(std::move(cb));
//		return true;
//	}
//
//	return false;
//}
//
//std::shared_ptr<IProperty> PropertyManager::addProperty(const uuid& self, std::shared_ptr<IProperty> pProperty)
//{
//	const std::string& strProperty = pProperty->getKey();
//	std::shared_ptr<IProperty> pOldProperty = this->getElement(strProperty);
//	if (!pOldProperty)
//	{
//		std::shared_ptr<IProperty> pNewProperty(new Property(self, strProperty, pProperty->getType()));
//
//		pNewProperty->setPublic(pProperty->getPublic());
//		pNewProperty->setPrivate(pProperty->getPrivate());
//		pNewProperty->setSave(pProperty->getSave());
//		pNewProperty->setCache(pProperty->getCache());
//		pNewProperty->setRef(pProperty->getRef());
//		pNewProperty->setUpload(pProperty->getUpload());
//
//		this->addElement(strProperty, pNewProperty);
//	}
//
//	return pOldProperty;
//}
//
//std::shared_ptr<IProperty> PropertyManager::addProperty(const uuid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType)
//{
//	std::shared_ptr<IProperty> pProperty = this->getElement(strPropertyName);
//	if (!pProperty)
//	{
//		pProperty = std::shared_ptr<IProperty>(new Property(self, strPropertyName, varType));
//		this->addElement(strPropertyName, pProperty);
//	}
//
//	return pProperty;
//}
//
//bool PropertyManager::setProperty(const std::string& strPropertyName, const VariantData& TData)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		pProperty->setValue(TData);
//
//		return true;
//	}
//
//	return false;
//}
//
//const uuid& PropertyManager::self()
//{
//	return mSelf;
//}
//
//bool PropertyManager::setPropertyInt(const std::string& strPropertyName, const int64 nValue)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->setInt(nValue);
//	}
//
//	return false;
//}
//
//bool PropertyManager::setPropertyDouble(const std::string& strPropertyName, const double dwValue)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->setDouble(dwValue);
//	}
//
//	return false;
//}
//
//bool PropertyManager::setPropertyString(const std::string& strPropertyName, const std::string& strValue)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->setString(strValue);
//	}
//
//	return false;
//}
//
//bool PropertyManager::setPropertyObject(const std::string& strPropertyName, const uuid& obj)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->setObject(obj);
//	}
//
//	return false;
//}
//
//int64 PropertyManager::getPropertyInt(const std::string& strPropertyName)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	return pProperty ? pProperty->getInt() : 0;
//}
//
//double PropertyManager::getPropertyDouble(const std::string& strPropertyName)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	return pProperty ? pProperty->getDouble() : 0.0;
//}
//
//const std::string& PropertyManager::getPropertyString(const std::string& strPropertyName)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->getString();
//	}
//
//	return NULL_STR;
//}
//
//const uuid& PropertyManager::getPropertyObject(const std::string& strPropertyName)
//{
//	std::shared_ptr<IProperty> pProperty = getElement(strPropertyName);
//	if (pProperty)
//	{
//		return pProperty->getObject();
//	}
//
//	return NULL_OBJECT;
//}
//
