// -------------------------------------------------------------------------




//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTY_MODULE_H
#define NFI_PROPERTY_MODULE_H

#include <iostream>
#include "IModule.h"

class NFIPropertyModule
    : public IModule
{
public:
    enum NFPropertyGroup
    {
		NPG_JOBLEVEL,
		NPG_FIGHTING_HERO,
		NPG_EFFECTVALUE,
		NPG_REBIRTH_ADD,
		NPG_EQUIP,
		NPG_EQUIP_AWARD,
		NPG_STATIC_BUFF,
		NPG_RUNTIME_BUFF,
        NPG_ALL ,
    };


    virtual int RefreshBaseProperty(const Guid& self) = 0;
    virtual int64 GetPropertyValue(const Guid& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType) = 0;
    virtual int SetPropertyValue(const Guid& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int64 nValue) = 0;
    virtual bool CalculatePropertyValue(const Guid& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int64 nValue, const bool bPositive = false) = 0;

    virtual bool FullHPMP(const Guid& self) = 0;
    virtual bool AddHP(const Guid& self, const int nValue) = 0;
    virtual bool ConsumeHP(const Guid& self, const int nValue) = 0;
    virtual bool EnoughHP(const Guid& self, const int nValue) = 0;
	virtual bool DamageHP(const Guid& self, const int nValue) = 0;

    virtual bool AddMP(const Guid& self, const int nValue) = 0;
    virtual bool ConsumeMP(const Guid& self, const int nValue) = 0;
    virtual bool EnoughMP(const Guid& self, const int nValue) = 0;
	virtual bool DamageMP(const Guid& self, const int nValue) = 0;

    virtual bool FullSP(const Guid& self) = 0;
    virtual bool AddSP(const Guid& self, const int nValue) = 0;
    virtual bool ConsumeSP(const Guid& self, const int nValue) = 0;
    virtual bool EnoughSP(const Guid& self, const int nValue) = 0;

    virtual bool AddGold(const Guid& self, const int64 nValue) = 0;
    virtual bool ConsumeGold(const Guid& self, const int64 nValue) = 0;
    virtual bool EnoughGold(const Guid& self, const int64 nValue) = 0;

    virtual bool AddDiamond(const Guid& self, const int nValue) = 0;
    virtual bool ConsumeDiamond(const Guid& self, const int nValue) = 0;
    virtual bool EnoughDiamond(const Guid& self, const int nValue) = 0;
};

#endif