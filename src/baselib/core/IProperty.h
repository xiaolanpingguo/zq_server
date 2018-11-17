////#pragma once
////
////
////#include "interface_header/base/platform.h"
////
////namespace zq {
////
////using PropertyEventFunT = std::function<int(const Guid&, const std::string&, const VariantData&, const VariantData&)>;
////class IProperty :public MemoryCounter<IProperty>
////{
////public:
////	IProperty() : MemoryCounter(GET_CLASS_NAME(IProperty))
////	{
////	}
////
////	virtual ~IProperty() {}
////
////	virtual void setValue(const VariantData& TData) = 0;
////	virtual void setValue(const IProperty* pProperty) = 0;
////
////	virtual bool setInt(const int64 value) = 0;
////	virtual bool setDouble(const double value) = 0;
////	virtual bool setString(const std::string& value) = 0;
////	virtual bool setObject(const Guid& value) = 0;
////
////	virtual int64 getInt() const = 0;
////	virtual double getDouble() const = 0;
////	virtual const std::string& getString() const = 0;
////	virtual const Guid& getObject() const = 0;
////
////	virtual const EN_DATA_TYPE getType() const = 0;
////	virtual const bool getUsed() const = 0;
////	virtual const std::string& getKey() const = 0;
////
////	virtual const bool getSave() const = 0;
////	virtual const bool getPublic() const = 0;
////	virtual const bool getPrivate() const = 0;
////	virtual const bool getCache() const = 0;
////	virtual const bool getRef() const = 0;
////	virtual const bool getForce() const = 0;
////	virtual const bool getUpload() const = 0;
////
////	virtual void setSave(bool bSave) = 0;
////	virtual void setPublic(bool bPublic) = 0;
////	virtual void setPrivate(bool bPrivate) = 0;
////	virtual void setCache(bool bCache) = 0;
////	virtual void setRef(bool bRef) = 0;
////	virtual void setForce(bool bRef) = 0;
////	virtual void setUpload(bool bUpload) = 0;
////
////	virtual const VariantData& getValue() const = 0;
////	virtual const std::shared_ptr<std::list<std::string>> getEmbeddedList() const = 0;
////	virtual const std::shared_ptr<MapEx<std::string, std::string>> getEmbeddedMap() const = 0;
////
////	virtual bool changed() const = 0;
////
////	virtual std::string toString() = 0;
////	virtual bool fromString(const std::string& strData) = 0;
////	virtual bool deSerialization() = 0;
////
////	virtual void registerCallback(PropertyEventFunT&& cb) = 0;
////};
////
////}
