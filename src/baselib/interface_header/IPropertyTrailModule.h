// -------------------------------------------------------------------------
//    @FileName			:    NFIPropertyTrailModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2013-09-30
//    @Module           :    NFIPropertyTrailModule
//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTY_TRAIL_MODULE_H
#define NFI_PROPERTY_TRAIL_MODULE_H

#include <iostream>
#include "IModule.h"

class NFIPropertyTrailModule
    : public IModule
{
public:
    virtual void StartTrail(const Guid self) = 0;
    virtual void EndTrail(const Guid self) = 0;

};


#endif
