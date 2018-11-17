#include "kernel_module.h"
#include "baselib/core/object.h"
#include "interface_header/base/uuid.h"
#include "baselib/message/config_define.hpp"
#include <random>

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <windows.h>
#endif

namespace zq{

KernelModule::KernelModule(ILibManager* p)
{
    guidIndex_ = 0;
    randomPos_ = 0;
    lastActTime_ = 0;

    libManager_ = p;

    lastActTime_ = libManager_->getNowTime();
}

KernelModule::~KernelModule()
{
}

void KernelModule::initRandom()
{
    vecRandom_.clear();

    int nRandomMax = 100000;
    randomPos_ = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1.0f);

    for (int i = 0; i < nRandomMax; i++)
    {
        vecRandom_.push_back((float) dis(gen));
    }
}

bool KernelModule::init()
{
    classModule_ = libManager_->findModule<IClassModule>();
    logModule_ = libManager_->findModule<ILogModule>();

	initRandom();

    return true;
}

bool KernelModule::initEnd()
{
	using namespace std::placeholders;


	return true;
}


bool KernelModule::shut()
{
	destroyAll();
	vecRandom_.clear();

    return true;
}

bool KernelModule::run()
{
    return true;
}

std::shared_ptr<IObject> KernelModule::createObject(const std::string& class_name)
{
	using namespace std::placeholders;

	//// 这里应该不可能存在，不然这个函数就出大问题了
 //   Guid ident = createGUID();
 //   if (existObject(ident))
 //   {
 //       LOG_ERROR << "The object has Exists, id: " << ident.toString();
 //       return nullptr;
 //   }
	//  
	//IObjectPtr pObject = std::make_shared<CObject>();
	//objs_[ident] = pObject;
	//

 //   return pObject;

	return nullptr;
}

bool KernelModule::destroyObject(const Guid& self)
{
    return false;
}

Guid KernelModule::createGUID()
{
    int64 value = 0;   
    uint64 time = getTimestampMs();

    
    //value = time << 16;
    value = time * 1000000;

    
    //value |= guidIndex_++;
    value += guidIndex_++;

    //if (sequence_ == 0x7FFF)
    if (guidIndex_ == 999999)
    {
        guidIndex_ = 0;
    }

    Guid xID;
    xID.nHead64 = libManager_->getServerID();
    xID.nData64 = value;

    return xID;
}

int KernelModule::onPropertyCommonEvent(const Guid& self, const std::string& strPropertyName, const VariantData& oldVar, const VariantData& newVar)
{
	/*std::shared_ptr<IObject> xObject = getElement(self);
	if (xObject)
	{
		if (xObject->getState() >= CLASS_OBJECT_EVENT::COE_CREATE_HASDATA)
		{
			for (auto it = mtCommonPropertyCallBackList.begin(); it != mtCommonPropertyCallBackList.end(); ++it)
			{
				(*it)(self, strPropertyName, oldVar, newVar);
			}

			const std::string& strClassName = xObject->getPropertyString(config::Object::class_name());
			auto itClass = mtClassPropertyCallBackList.find(strClassName);
			if (itClass != mtClassPropertyCallBackList.end())
			{
				for (auto itList = itClass->second.begin(); itList != itClass->second.end(); itList++)
				{
					(*itList)(self, strPropertyName, oldVar, newVar);
				}
			}
		}
	}*/
	
    return 0;
}

std::shared_ptr<IObject> KernelModule::getObject(const Guid& ident)
{	
	//auto it = objs_.find(ident);
	//if (it == objs_.end())
	//{
	//	return nullptr;
	//}

 //   return it->second;

	return nullptr;
}

bool KernelModule::existObject(const Guid & ident)
{
	//auto it = objs_.find(ident);
	//if (it == objs_.end())
	//{
	//	return false;
	//}

	return true;
}

bool KernelModule::destroyAll()
{

    return true;
}



}