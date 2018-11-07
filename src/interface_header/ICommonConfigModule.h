// -------------------------------------------------------------------------




//
// -------------------------------------------------------------------------

#ifndef NFI_COMMONCONFIG_MODULE_H
#define NFI_COMMONCONFIG_MODULE_H

#include <iostream>
#include "NFIBuffConfigModule.h"


class NFICommonConfigModule
    : public IModule
{
public:
    virtual bool ClearConfig() = 0;
    virtual const int GetAttributeInt(const std::string& strStructName, const std::string& strStructItemName, const std::string& strAttribute) = 0;
    virtual const int GetAttributeInt(const std::string& strStructName, const std::string& strSDKAttribute) = 0;
    virtual const std::string& GetAttributeString(const std::string& strStructName, const std::string& strStructItemName, const std::string& strAttribute) = 0;
    virtual const std::string& GetAttributeString(const std::string& strStructName, const std::string& strSDKAttribute) = 0;
    virtual bool LoadConfig(const std::string& strFile) = 0;
    virtual std::vector<std::string> GetStructItemList(const std::string& strStructName) = 0;
};

#endif