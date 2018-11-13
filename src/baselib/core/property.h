#pragma once

#include <list>
#include "map_ex.hpp"
#include "data_list.hpp"
#include "IProperty.h"

namespace zq {

class Property : public IProperty
{
private:
	Property();

public:
	Property(const Guid& self, const std::string& strPropertyName, const EN_DATA_TYPE varType);

	virtual ~Property();

	virtual void setValue(const VariantData& TData);
	virtual void setValue(const IProperty* pProperty);

	virtual bool setInt(const int64 value);
	virtual bool setDouble(const double value);
	virtual bool setString(const std::string& value);
	virtual bool setObject(const Guid& value);

	virtual int64 getInt() const;
	virtual double getDouble() const;
	virtual const std::string& getString() const;
	virtual const Guid& getObject() const;

	virtual const EN_DATA_TYPE getType() const;
	virtual const bool getUsed() const;
	virtual const std::string& getKey() const;
	virtual const bool getSave() const;
	virtual const bool getPublic() const;
	virtual const bool getPrivate() const;
	virtual const bool getCache() const;
	virtual const bool getRef() const;
	virtual const bool getForce() const;
	virtual const bool getUpload() const;

	virtual void setSave(bool bSave);
	virtual void setPublic(bool bPublic);
	virtual void setPrivate(bool bPrivate);
	virtual void setCache(bool bCache);
	virtual void setRef(bool bRef);
	virtual void setForce(bool bRef);
	virtual void setUpload(bool bUpload);

	virtual const VariantData& getValue() const;
	virtual const std::shared_ptr<std::list<std::string>> getEmbeddedList() const;
	virtual const std::shared_ptr<MapEx<std::string, std::string>> getEmbeddedMap() const;

	virtual bool changed() const;

	virtual std::string toString();
	virtual bool fromString(const std::string& strData);
	virtual bool deSerialization();

	virtual void registerCallback(PROPERTY_EVENT_FUNCTOR&& cb);

private:
	int onEventHandler(const VariantData& oldVar, const VariantData& newVar);

private:
	using VecPropertyCbT = std::vector<PROPERTY_EVENT_FUNCTOR>;
	VecPropertyCbT propertyCb_;

	Guid self_;
	std::string propertyName_;
	EN_DATA_TYPE dataType_;

	std::shared_ptr<VariantData> variantData_;
	std::shared_ptr<MapEx<std::string, std::string>> embeddedMap_;
	std::shared_ptr<std::list<std::string>> embeddedList_;

	bool public_;
	bool private_;
	bool save_;
	bool cache_;
	bool ref_;
	bool force_;
	bool upload_;
};

}
