#pragma once

#include "platform.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace zq{

struct Guid
{
    int64 nData64;
    int64 nHead64;

    Guid()
    {
        nData64 = 0;
        nHead64 = 0;
    }

    Guid(int64 nHeadData, int64 nData)
    {
        nHead64 = nHeadData;
        nData64 = nData;
    }

    Guid(const Guid& xData)
    {
        nHead64 = xData.nHead64;
        nData64 = xData.nData64;
    }

    Guid& operator=(const Guid& xData)
    {
        nHead64 = xData.nHead64;
        nData64 = xData.nData64;

        return *this;
    }

    const int64 getData() const
    {
        return nData64;
    }

    const int64 getHead() const
    {
        return nHead64;
    }

    void setData(const int64 nData)
    {
        nData64 = nData;
    }

    void setHead(const int64 nData)
    {
        nHead64 = nData;
    }

    bool isNull() const
    {
        return 0 == nData64 && 0 == nHead64;
    }

    bool operator == (const Guid& id) const
    {
        return this->nData64 == id.nData64 && this->nHead64 == id.nHead64;
    }

    bool operator != (const Guid& id) const
    {
        return this->nData64 != id.nData64 || this->nHead64 != id.nHead64;
    }

    bool operator < (const Guid& id) const
    {
        if (this->nHead64 == id.nHead64)
        {
            return this->nData64 < id.nData64;
        }

        return this->nHead64 < id.nHead64;
    }

    std::string toString() const
    {
        return lexical_cast<std::string>(nHead64) + "-" + lexical_cast<std::string>(nData64);
    }

    bool fromString(const std::string& strID)
    {
        size_t nStrLength = strID.length();
        size_t nPos = strID.find('-');
        if (nPos == std::string::npos)
        {
            return false;
        }

        std::string strHead = strID.substr(0, nPos);
        std::string strData = "";
        if (nPos + 1 < nStrLength)
        {
            strData = strID.substr(nPos + 1, nStrLength - nPos);
        }

        try
        {
            nHead64 = lexical_cast<int64>(strHead);
            nData64 = lexical_cast<int64>(strData);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

}
