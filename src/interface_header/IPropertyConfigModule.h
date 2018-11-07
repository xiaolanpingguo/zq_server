// -------------------------------------------------------------------------
//    @FileName			:    NFIPropertyConfigModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2013-09-30
//    @Module           :    NFIPropertyConfigModule
//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTY_CONFIG_MODULE_H
#define NFI_PROPERTY_CONFIG_MODULE_H

#include <iostream>
#include "IModule.h"

class NFIPropertyConfigModule
    : public IModule
{
public:

    virtual int64 CalculateBaseValue(const int nJob,  const int nLevel, const std::string& strProperty) = 0;
    virtual bool LegalLevel(const int nJob, const int nLevel) = 0;
};


#endif
