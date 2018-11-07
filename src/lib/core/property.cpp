#include "property.h"
#include <complex>
using namespace zq;

Property::Property()
{
	public_ = false;
	private_ = false;
	save_ = false;
	cache_ = false;
	ref_ = false;
	force_ = false;
	upload_ = false;

	self_ = Guid();
	dataType_ = TDATA_UNKNOWN;

	propertyName_ = "";
}

Property::Property(const Guid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType)
{
	public_ = false;
	private_ = false;
	save_ = false;
	cache_ = false;
	ref_ = false;
	upload_ = false;

	self_ = self;

	propertyName_ = strPropertyName;
	dataType_ = varType;
}

Property::~Property()
{
	propertyCb_.clear();
	abstractData_.reset();
}

void Property::setValue(const AbstractData& xData)
{
	if (dataType_ != xData.getType() || xData.getType() == EN_DATA_TYPE::TDATA_UNKNOWN)
	{
		return;
	}

	if (xData.IsNullValue())
	{
		return;
	}

	if (nullptr == abstractData_)
	{
		abstractData_ = std::shared_ptr<AbstractData>(new AbstractData(xData));
	}

	if (propertyCb_.size() == 0)
	{
		abstractData_->variantData = xData.variantData;
	}
	else
	{
		// 值变化了，回调事件
		AbstractData oldValue;
		oldValue = *abstractData_;

		abstractData_->variantData = xData.variantData;

		AbstractData newValue;
		newValue = *abstractData_;

		onEventHandler(oldValue, newValue);
	}

}

void Property::setValue(const IProperty* pProperty)
{
	setValue(pProperty->getValue());
}

const AbstractData& Property::getValue() const
{
	if (abstractData_)
	{
		return *abstractData_;
	}

	return NULL_TDATA;
}

const std::string&  Property::getKey() const
{
	return propertyName_;
}

const bool Property::getSave() const
{
	return save_;
}

const bool Property::getPublic() const
{
	return public_;
}

const bool Property::getPrivate() const
{
	return private_;
}

const bool Property::getCache() const
{
	return cache_;
}

const bool Property::getRef() const
{
	return ref_;
}

const bool Property::getForce() const
{
	return force_;
}

const bool Property::getUpload() const
{
	return upload_;
}

void Property::setSave(bool bSave)
{
	save_ = bSave;
}

void Property::setPublic(bool bPublic)
{
	public_ = bPublic;
}

void Property::setPrivate(bool bPrivate)
{
	private_ = bPrivate;
}

void Property::setCache(bool bCache)
{
    cache_ = bCache;
}

void Property::setRef(bool bRef)
{
	ref_ = bRef;
}

void Property::setForce(bool bRef)
{
	force_ = bRef;
}

void Property::setUpload(bool bUpload)
{
	upload_ = bUpload;
}

int64 Property::getInt() const
{
	if (!abstractData_)
	{
		return 0;
	}

	return abstractData_->getInt();
}

double Property::getFloat() const
{
	if (!abstractData_)
	{
		return 0.0;
	}

	return abstractData_->getFloat();
}

const std::string& Property::getString() const
{
	if (!abstractData_)
	{
		return NULL_STR;
	}

	return abstractData_->getString();
}

const Guid& Property::getObject() const
{
	if (!abstractData_)
	{
		return NULL_OBJECT;
	}

	return abstractData_->getObject();
}

void Property::registerCallback(PROPERTY_EVENT_FUNCTOR&& cb)
{
	propertyCb_.push_back(cb);
}

int Property::onEventHandler(const AbstractData& oldVar, const AbstractData& newVar)
{
	if (propertyCb_.size() <= 0)
	{
		return 0;
	}

	for (auto it = propertyCb_.begin(); it != propertyCb_.end(); ++it)
	{
		(*it)(self_, propertyName_, oldVar, newVar);
	}

	return 0;
}

bool Property::setInt(const int64 value)
{
	if (dataType_ != TDATA_INT)
	{
		return false;
	}

	if (!abstractData_)
	{
		
		if (0 == value)
		{
			return false;
		}

		abstractData_ = std::shared_ptr<AbstractData>(new AbstractData(TDATA_INT));
		abstractData_->setInt(0);
	}

	if (value == abstractData_->getInt())
	{
		return false;
	}

	if (propertyCb_.size() == 0)
	{
		abstractData_->setInt(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue = *abstractData_;

		abstractData_->setInt(value);

		onEventHandler(oldValue, *abstractData_);
	}

	return true;
}

bool Property::setFloat(const double value)
{
	if (dataType_ != TDATA_FLOAT)
	{
		return false;
	}

	if (!abstractData_)
	{
		
		if (isZeroDouble(value))
		{
			return false;
		}

		abstractData_ = std::shared_ptr<AbstractData>(new AbstractData(TDATA_FLOAT));
		abstractData_->setFloat(0.0);
	}

	if (isZeroDouble(value - abstractData_->getFloat()))
	{
		return false;
	}

	if (propertyCb_.size() == 0)
	{
		abstractData_->setFloat(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue = *abstractData_;

		abstractData_->setFloat(value);

		onEventHandler(oldValue, *abstractData_);
	}

	return true;
}

bool Property::setString(const std::string& value)
{
	if (dataType_ != TDATA_STRING)
	{
		return false;
	}

	if (!abstractData_)
	{
		
		if (value.empty())
		{
			return false;
		}

		abstractData_ = std::shared_ptr<AbstractData>(new AbstractData(TDATA_STRING));
		abstractData_->setString(NULL_STR);
	}

	if (value == abstractData_->getString())
	{
		return false;
	}

	if (propertyCb_.size() == 0)
	{
		abstractData_->setString(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue = *abstractData_;

		abstractData_->setString(value);

		onEventHandler(oldValue, *abstractData_);
	}

	return true;
}

bool Property::setObject(const Guid& value)
{
	if (dataType_ != TDATA_OBJECT)
	{
		return false;
	}

	if (!abstractData_)
	{
		
		if (value.isNull())
		{
			return false;
		}

		abstractData_ = std::shared_ptr<AbstractData>(new AbstractData(TDATA_OBJECT));
		abstractData_->setObject(Guid());
	}

	if (value == abstractData_->getObject())
	{
		return false;
	}

	if (propertyCb_.size() == 0)
	{
		abstractData_->setObject(value);
	}
	else
	{
		AbstractData oldValue;
		oldValue = *abstractData_;

		abstractData_->setObject(value);

		onEventHandler(oldValue, *abstractData_);
	}

	return true;
}

bool Property::changed() const
{
	return !(getValue().IsNullValue());
}

const EN_DATA_TYPE Property::getType() const
{
	return dataType_;
}

const bool Property::getUsed() const
{
	if (abstractData_)
	{
		return true;
	}

	return false;
}

std::string Property::toString()
{
	std::string strData;
	const EN_DATA_TYPE dataType_ = getType();
	switch (dataType_)
	{
	case TDATA_INT:
		strData = lexical_cast<std::string> (getInt());
		break;
	case TDATA_FLOAT:
		strData = lexical_cast<std::string> (getFloat());
		break;
	case TDATA_STRING:
		strData = getString();
		break;
	case TDATA_OBJECT:
		strData = getObject().toString();
		break;
	default:
		strData = NULL_STR;
		break;
	}

	return strData;
}

bool Property::fromString(const std::string& strData)
{
	try
	{
		switch (getType())
		{
		case TDATA_INT:
			setInt(lexical_cast<int64> (strData));
			break;

		case TDATA_FLOAT:
			setFloat(lexical_cast<float> (strData));
			break;

		case TDATA_STRING:
			setString(strData);
			break;

		case TDATA_OBJECT:
		{
			Guid xID;
			xID.fromString(strData);
			setObject(xID);
		}
		break;
		default:
			break;
		}

		return true;
	}
	catch (...)
	{
		return false;
	}

	return false;
}

bool Property::deSerialization()
{
	bool bRet = false;

	const EN_DATA_TYPE dataType_ = getType();
	if (dataType_ == TDATA_STRING && nullptr != abstractData_ && !abstractData_->IsNullValue())
	{
		DataList xDataList;
		const std::string& strData = abstractData_->getString();

		xDataList.Split(strData.c_str(), ";");
		if (xDataList.GetCount() <= 0)
		{
			return bRet;
		}

		DataList xTemDataList;
		xTemDataList.Split(xDataList.String(0).c_str(), ",");
		const int nSubDataLength = xTemDataList.GetCount();

		if (xDataList.GetCount() == 1 && nSubDataLength == 1)
		{
			//most of property value only one value
			return bRet;
		}

		if (nullptr == embeddedList_)
		{
			embeddedList_ = std::shared_ptr<std::list<std::string>>(new std::list<std::string>());
		}
		else
		{
			embeddedList_->clear();
		}

		for (int i = 0; i < xDataList.GetCount(); ++i)
		{
			if (xDataList.String(i).empty())
			{
				ASSERT(false);
			}

			embeddedList_->emplace_back(xDataList.String(i));
		}

		if (nSubDataLength < 2 || nSubDataLength > 2)
		{
			return bRet;
		}

		if (nullptr == embeddedMap_)
		{
			embeddedMap_ = std::shared_ptr<MapEx<std::string, std::string>>(new MapEx<std::string, std::string>());
		}
		else
		{
			embeddedMap_->clear();
		}

		for (int i = 0; i < xDataList.GetCount(); ++i)
		{
			DataList xTemDataList;
			const std::string& strTemData = xDataList.String(i);
			xTemDataList.Split(strTemData.c_str(), ",");
			{
				if (xTemDataList.GetCount() != nSubDataLength)
				{
					ASSERT(false);
				}

				const std::string& strKey = xTemDataList.String(0);
				const std::string& strValue = xTemDataList.String(1);

				if (strKey.empty() || strValue.empty())
				{
					ASSERT(false);
				}

				embeddedMap_->addElement(strKey, std::shared_ptr<std::string>(new std::string(strValue)));
			}
		}

		bRet = true;
	}

	return bRet;
}

const std::shared_ptr<std::list<std::string>> Property::getEmbeddedList() const
{
	return this->embeddedList_;
}

const std::shared_ptr<MapEx<std::string, std::string>> Property::getEmbeddedMap() const
{
	return this->embeddedMap_;
}
