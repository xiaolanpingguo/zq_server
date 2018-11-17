//#include "property.h"
//#include <complex>
//using namespace zq;
//
//Property::Property()
//{
//	public_ = false;
//	private_ = false;
//	save_ = false;
//	cache_ = false;
//	ref_ = false;
//	force_ = false;
//	upload_ = false;
//
//	self_ = Guid();
//	dataType_ = TDATA_UNKNOWN;
//
//	propertyName_ = "";
//}
//
//Property::Property(const Guid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType)
//{
//	public_ = false;
//	private_ = false;
//	save_ = false;
//	cache_ = false;
//	ref_ = false;
//	upload_ = false;
//
//	self_ = self;
//
//	propertyName_ = strPropertyName;
//	dataType_ = varType;
//}
//
//Property::~Property()
//{
//	propertyCb_.clear();
//	variantData_.reset();
//}
//
//void Property::setValue(const VariantData& xData)
//{
//	if (dataType_ != xData.getType() || xData.getType() == EN_DATA_TYPE::TDATA_UNKNOWN)
//	{
//		return;
//	}
//
//	if (xData.IsNullValue())
//	{
//		return;
//	}
//
//	if (nullptr == variantData_)
//	{
//		variantData_ = std::shared_ptr<VariantData>(new VariantData(xData));
//	}
//
//	if (propertyCb_.size() == 0)
//	{
//		variantData_->variantData = xData.variantData;
//	}
//	else
//	{
//		// 值变化了，回调事件
//		VariantData oldValue;
//		oldValue = *variantData_;
//
//		variantData_->variantData = xData.variantData;
//
//		VariantData newValue;
//		newValue = *variantData_;
//
//		onEventHandler(oldValue, newValue);
//	}
//
//}
//
//void Property::setValue(const IProperty* pProperty)
//{
//	setValue(pProperty->getValue());
//}
//
//const VariantData& Property::getValue() const
//{
//	if (variantData_)
//	{
//		return *variantData_;
//	}
//
//	return NULL_TDATA;
//}
//
//const std::string&  Property::getKey() const
//{
//	return propertyName_;
//}
//
//const bool Property::getSave() const
//{
//	return save_;
//}
//
//int64 Property::getInt() const
//{
//	if (!variantData_)
//	{
//		return 0;
//	}
//
//	return variantData_->getInt();
//}
//
//double Property::getDouble() const
//{
//	if (!variantData_)
//	{
//		return 0.0;
//	}
//
//	return variantData_->getDouble();
//}
//
//const std::string& Property::getString() const
//{
//	if (!variantData_)
//	{
//		return NULL_STR;
//	}
//
//	return variantData_->getString();
//}
//
//const Guid& Property::getObject() const
//{
//	if (!variantData_)
//	{
//		return NULL_OBJECT;
//	}
//
//	return variantData_->getObject();
//}
//
//void Property::registerCallback(PropertyEventFunT&& cb)
//{
//	propertyCb_.push_back(cb);
//}
//
//int Property::onEventHandler(const VariantData& oldVar, const VariantData& newVar)
//{
//	if (propertyCb_.size() <= 0)
//	{
//		return 0;
//	}
//
//	for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
//	{
//		(*it)(self_, propertyName_, oldVar, newVar);
//	}
//
//	return 0;
//}
//
//bool Property::setInt(const int64 value)
//{
//	if (dataType_ != TDATA_INT)
//	{
//		return false;
//	}
//
//	if (!variantData_)
//	{
//		
//		if (0 == value)
//		{
//			return false;
//		}
//
//		variantData_ = std::shared_ptr<VariantData>(new VariantData(TDATA_INT));
//		variantData_->setInt(0);
//	}
//
//	if (value == variantData_->getInt())
//	{
//		return false;
//	}
//
//	if (propertyCb_.size() == 0)
//	{
//		variantData_->setInt(value);
//	}
//	else
//	{
//		VariantData oldValue;
//		oldValue = *variantData_;
//
//		variantData_->setInt(value);
//
//		onEventHandler(oldValue, *variantData_);
//	}
//
//	return true;
//}
//
//bool Property::setDouble(const double value)
//{
//	if (dataType_ != TDATA_FLOAT)
//	{
//		return false;
//	}
//
//	if (!variantData_)
//	{
//		
//		if (isZeroDouble(value))
//		{
//			return false;
//		}
//
//		variantData_ = std::shared_ptr<VariantData>(new VariantData(TDATA_FLOAT));
//		variantData_->setDouble(0.0);
//	}
//
//	if (isZeroDouble(value - variantData_->getDouble()))
//	{
//		return false;
//	}
//
//	if (propertyCb_.size() == 0)
//	{
//		variantData_->setDouble(value);
//	}
//	else
//	{
//		VariantData oldValue;
//		oldValue = *variantData_;
//
//		variantData_->setDouble(value);
//
//		onEventHandler(oldValue, *variantData_);
//	}
//
//	return true;
//}
//
//bool Property::setString(const std::string& value)
//{
//	if (dataType_ != TDATA_STRING)
//	{
//		return false;
//	}
//
//	if (!variantData_)
//	{
//		
//		if (value.empty())
//		{
//			return false;
//		}
//
//		variantData_ = std::shared_ptr<VariantData>(new VariantData(TDATA_STRING));
//		variantData_->setString(NULL_STR);
//	}
//
//	if (value == variantData_->getString())
//	{
//		return false;
//	}
//
//	if (propertyCb_.size() == 0)
//	{
//		variantData_->setString(value);
//	}
//	else
//	{
//		VariantData oldValue;
//		oldValue = *variantData_;
//
//		variantData_->setString(value);
//
//		onEventHandler(oldValue, *variantData_);
//	}
//
//	return true;
//}
//
//bool Property::setObject(const Guid& value)
//{
//	if (dataType_ != TDATA_OBJECT)
//	{
//		return false;
//	}
//
//	if (!variantData_)
//	{
//		
//		if (value.isNull())
//		{
//			return false;
//		}
//
//		variantData_ = std::shared_ptr<VariantData>(new VariantData(TDATA_OBJECT));
//		variantData_->setObject(Guid());
//	}
//
//	if (value == variantData_->getObject())
//	{
//		return false;
//	}
//
//	if (propertyCb_.size() == 0)
//	{
//		variantData_->setObject(value);
//	}
//	else
//	{
//		VariantData oldValue;
//		oldValue = *variantData_;
//
//		variantData_->setObject(value);
//
//		onEventHandler(oldValue, *variantData_);
//	}
//
//	return true;
//}
//
//bool Property::changed() const
//{
//	return !(getValue().IsNullValue());
//}
//
//const EN_DATA_TYPE Property::getType() const
//{
//	return dataType_;
//}
//
//const bool Property::getUsed() const
//{
//	if (variantData_)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//std::string Property::toString()
//{
//	std::string strData;
//	const EN_DATA_TYPE dataType_ = getType();
//	switch (dataType_)
//	{
//	case TDATA_INT:
//		strData = lexical_cast<std::string> (getInt());
//		break;
//	case TDATA_FLOAT:
//		strData = lexical_cast<std::string> (getDouble());
//		break;
//	case TDATA_STRING:
//		strData = getString();
//		break;
//	case TDATA_OBJECT:
//		strData = getObject().toString();
//		break;
//	default:
//		strData = NULL_STR;
//		break;
//	}
//
//	return strData;
//}
//
//bool Property::fromString(const std::string& strData)
//{
//	try
//	{
//		switch (getType())
//		{
//		case TDATA_INT:
//			setInt(lexical_cast<int64> (strData));
//			break;
//
//		case TDATA_FLOAT:
//			setDouble(lexical_cast<float> (strData));
//			break;
//
//		case TDATA_STRING:
//			setString(strData);
//			break;
//
//		case TDATA_OBJECT:
//		{
//			Guid xID;
//			xID.fromString(strData);
//			setObject(xID);
//		}
//		break;
//		default:
//			break;
//		}
//
//		return true;
//	}
//	catch (...)
//	{
//		return false;
//	}
//
//	return false;
//}
//
//bool Property::deSerialization()
//{
//	bool bRet = false;
//
//	const EN_DATA_TYPE dataType_ = getType();
//	if (dataType_ == TDATA_STRING && nullptr != variantData_ && !variantData_->IsNullValue())
//	{
//		DataList xDataList;
//		const std::string& strData = variantData_->getString();
//
//		xDataList.Split(strData.c_str(), ";");
//		if (xDataList.GetCount() <= 0)
//		{
//			return bRet;
//		}
//
//		DataList xTemDataList;
//		xTemDataList.Split(xDataList.String(0).c_str(), ",");
//		const int nSubDataLength = xTemDataList.GetCount();
//
//		if (xDataList.GetCount() == 1 && nSubDataLength == 1)
//		{
//			//most of property value only one value
//			return bRet;
//		}
//
//		if (nullptr == embeddedList_)
//		{
//			embeddedList_ = std::shared_ptr<std::list<std::string>>(new std::list<std::string>());
//		}
//		else
//		{
//			embeddedList_->clear();
//		}
//
//		for (int i = 0; i < xDataList.GetCount(); ++i)
//		{
//			if (xDataList.String(i).empty())
//			{
//				ASSERT(false);
//			}
//
//			embeddedList_->emplace_back(xDataList.String(i));
//		}
//
//		if (nSubDataLength < 2 || nSubDataLength > 2)
//		{
//			return bRet;
//		}
//
//		if (nullptr == embeddedMap_)
//		{
//			embeddedMap_ = std::shared_ptr<MapEx<std::string, std::string>>(new MapEx<std::string, std::string>());
//		}
//		else
//		{
//			embeddedMap_->clear();
//		}
//
//		for (int i = 0; i < xDataList.GetCount(); ++i)
//		{
//			DataList xTemDataList;
//			const std::string& strTemData = xDataList.String(i);
//			xTemDataList.Split(strTemData.c_str(), ",");
//			{
//				if (xTemDataList.GetCount() != nSubDataLength)
//				{
//					ASSERT(false);
//				}
//
//				const std::string& strKey = xTemDataList.String(0);
//				const std::string& strValue = xTemDataList.String(1);
//
//				if (strKey.empty() || strValue.empty())
//				{
//					ASSERT(false);
//				}
//
//				embeddedMap_->addElement(strKey, std::shared_ptr<std::string>(new std::string(strValue)));
//			}
//		}
//
//		bRet = true;
//	}
//
//	return bRet;
//}
//
//const std::shared_ptr<std::list<std::string>> Property::getEmbeddedList() const
//{
//	return this->embeddedList_;
//}
//
//const std::shared_ptr<MapEx<std::string, std::string>> Property::getEmbeddedMap() const
//{
//	return this->embeddedMap_;
//}
