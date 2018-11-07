#include "kernel_module.h"
#include "baselib/core/object.h"
#include "baselib/core/record.h"
#include "baselib/interface_header/uuid.h"
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
    initRandom();
}

KernelModule::~KernelModule()
{
    clear();
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
    mtDeleteSelfList.clear();

    classModule_ = libManager_->findModule<IClassModule>();
    elementModule_ = libManager_->findModule<IElementModule>();
    logModule_ = libManager_->findModule<ILogModule>();
	m_pEventModule = libManager_->findModule<IEventModule>();

    return true;
}

bool KernelModule::shut()
{
    return true;
}

bool KernelModule::run()
{
    processMemFree();

    curExeObject_ = Guid();

    if (mtDeleteSelfList.size() > 0)
    {
        std::list<Guid>::iterator it = mtDeleteSelfList.begin();
        for (; it != mtDeleteSelfList.end(); it++)
        {
            destroyObject(*it);
        }
        mtDeleteSelfList.clear();
    }

    std::shared_ptr<IObject> pObject = first();
    while (pObject)
    {
        curExeObject_ = pObject->self();
        pObject->run();
        curExeObject_ = Guid();

        pObject = next();
    }

    return true;
}

std::shared_ptr<IObject> KernelModule::createObject(const Guid& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const DataList& arg)
{
	using namespace std::placeholders;

    std::shared_ptr<IObject> pObject;
    Guid ident = self;
   
	// 创建GUID
    if (ident.isNull())
    {
        ident = createGUID();
    }

	// 已经存在了
    if (getElement(ident))
    {
        logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, ident, "The object has Exists", __FUNCTION__, __LINE__);
        return pObject;
    }

    std::shared_ptr<IPropertyManager> pStaticClassPropertyManager = classModule_->getClassPropertyManager(strClassName);
    std::shared_ptr<IRecordManager> pStaticClassRecordManager = classModule_->getClassRecordManager(strClassName);
    if (pStaticClassPropertyManager && pStaticClassRecordManager)
    {
		// 创建对象并添加
        pObject = std::shared_ptr<IObject>(new CObject(ident, libManager_));
        addElement(ident, pObject);

        std::shared_ptr<IPropertyManager> pPropertyManager = pObject->getPropertyManager();
        std::shared_ptr<IRecordManager> pRecordManager = pObject->getRecordManager();
 
        std::shared_ptr<IProperty> pStaticConfigPropertyInfo = pStaticClassPropertyManager->first();
        while (pStaticConfigPropertyInfo)
        {
			// 创建一个属性，并根据配置来做相应的字段描述
            std::shared_ptr<IProperty> xProperty = pPropertyManager->addProperty(ident, pStaticConfigPropertyInfo->getKey(), pStaticConfigPropertyInfo->getType());

            xProperty->setPublic(pStaticConfigPropertyInfo->getPublic());
            xProperty->setPrivate(pStaticConfigPropertyInfo->getPrivate());
            xProperty->setSave(pStaticConfigPropertyInfo->getSave());
            xProperty->setCache(pStaticConfigPropertyInfo->getCache());
            xProperty->setRef(pStaticConfigPropertyInfo->getRef());
			xProperty->setUpload(pStaticConfigPropertyInfo->getUpload());
     
            pObject->addPropertyCallBack(pStaticConfigPropertyInfo->getKey(),
				std::bind(&KernelModule::onPropertyCommonEvent,this, _1, _2, _3, _4));

            pStaticConfigPropertyInfo = pStaticClassPropertyManager->next();
        }

        std::shared_ptr<IRecord> pConfigRecordInfo = pStaticClassRecordManager->first();
        while (pConfigRecordInfo)
        {
            std::shared_ptr<IRecord> xRecord =  pRecordManager->AddRecord(ident,
                                      pConfigRecordInfo->getName(),
                                      pConfigRecordInfo->getInitData(),
                                      pConfigRecordInfo->getTag(),
                                      pConfigRecordInfo->getRows());

             xRecord->setPublic(pConfigRecordInfo->getPublic());
             xRecord->setPrivate(pConfigRecordInfo->getPrivate());
             xRecord->setSave(pConfigRecordInfo->getSave());
             xRecord->setCache(pConfigRecordInfo->getCache());
			 xRecord->setUpload(pConfigRecordInfo->getUpload());
            
            pObject->addRecordCallBack(pConfigRecordInfo->getName(), 
				std::bind(&KernelModule::onRecordCommonEvent,this, _1, _2, _3, _4));

            pConfigRecordInfo = pStaticClassRecordManager->next();
        }

		pObject->setPropertyObject(config::Object::id(), self);
		pObject->setPropertyString(config::Object::config_id(), strConfigIndex);
		pObject->setPropertyString(config::Object::class_name(), strClassName);
		pObject->setPropertyInt(config::Object::scene_id(), nSceneID);
		pObject->setPropertyInt(config::Object::group_id(), nGroupID);

		// 此时对象已经创建了，但还没有数据
		doEvent(ident, strClassName, pObject->getState(), arg);

         // 这里开始填充数据
        std::shared_ptr<IPropertyManager> pConfigPropertyManager = elementModule_->getPropertyManager(strConfigIndex);
        std::shared_ptr<IRecordManager> pConfigRecordManager = elementModule_->getRecordManager(strConfigIndex);
        if (pConfigPropertyManager && pConfigRecordManager)
        {
            std::shared_ptr<IProperty> pConfigPropertyInfo = pConfigPropertyManager->first();
            while (pConfigPropertyInfo)
            {
                if (pConfigPropertyInfo->changed())
                {
                    pPropertyManager->setProperty(pConfigPropertyInfo->getKey(), pConfigPropertyInfo->getValue());
                }

                pConfigPropertyInfo = pConfigPropertyManager->next();
            }
        }

        for (int i = 0; i < arg.GetCount() - 1; i += 2)
        {
            const std::string& strPropertyName = arg.String(i);
            if (config::Object::config_id() != strPropertyName
                && config::Object::class_name() != strPropertyName
                && config::Object::scene_id() != strPropertyName
				&& config::Object::id() != strPropertyName
                && config::Object::group_id() != strPropertyName)
            {
                std::shared_ptr<IProperty> pArgProperty = pStaticClassPropertyManager->getElement(strPropertyName);
                if (pArgProperty)
                {
                    switch (pArgProperty->getType())
                    {
                        case TDATA_INT:
                            pObject->setPropertyInt(strPropertyName, arg.Int(i + 1));
                            break;
                        case TDATA_FLOAT:
                            pObject->setPropertyFloat(strPropertyName, arg.Float(i + 1));
                            break;
                        case TDATA_STRING:
                            pObject->setPropertyString(strPropertyName, arg.String(i + 1));
                            break;
                        case TDATA_OBJECT:
                            pObject->setPropertyObject(strPropertyName, arg.Object(i + 1));
                            break;
                        default:
                            break;
                    }
                }
            }
        }


		pObject->setState(COE_CREATE_BEFORE_ATTACHDATA);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_LOADDATA);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_AFTER_ATTACHDATA);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_BEFORE_EFFECT);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_EFFECTDATA);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_AFTER_EFFECT);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_HASDATA);
		doEvent(ident, strClassName, pObject->getState(), arg);

		pObject->setState(COE_CREATE_FINISH);
		doEvent(ident, strClassName, pObject->getState(), arg);
    }

    return pObject;
}

bool KernelModule::destroyObject(const Guid& self)
{
	 if (self == curExeObject_ && !self.isNull())
	 {
		 return destroySelf(self);
	 }

	 removeElement(self);

    return false;
}

bool KernelModule::findProperty(const Guid& self, const std::string& strPropertyName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->findProperty(strPropertyName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setPropertyInt(const Guid& self, const std::string& strPropertyName, const int64 nValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->setPropertyInt(strPropertyName, nValue);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setPropertyFloat(const Guid& self, const std::string& strPropertyName, const double dValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->setPropertyFloat(strPropertyName, dValue);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setPropertyString(const Guid& self, const std::string& strPropertyName, const std::string& strValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->setPropertyString(strPropertyName, strValue);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setPropertyObject(const Guid& self, const std::string& strPropertyName, const Guid& objectValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->setPropertyObject(strPropertyName, objectValue);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

int64 KernelModule::getPropertyInt(const Guid& self, const std::string& strPropertyName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getPropertyInt(strPropertyName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return NULL_INT;
}

double KernelModule::getPropertyFloat(const Guid& self, const std::string& strPropertyName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getPropertyFloat(strPropertyName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return NULL_FLOAT;
}

const std::string& KernelModule::getPropertyString(const Guid& self, const std::string& strPropertyName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getPropertyString(strPropertyName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return NULL_STR;
}

const Guid& KernelModule::getPropertyObject(const Guid& self, const std::string& strPropertyName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getPropertyObject(strPropertyName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strPropertyName + "| There is no object", __FUNCTION__, __LINE__);

    return NULL_OBJECT;
}

std::shared_ptr<IRecord> KernelModule::findRecord(const Guid& self, const std::string& strRecordName)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordManager()->getElement(strRecordName);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strRecordName + "| There is no object", __FUNCTION__, __LINE__);

    return nullptr;
}

bool KernelModule::clearRecord(const Guid& self, const std::string& strRecordName)
{
    std::shared_ptr<IRecord> pRecord =  findRecord(self, strRecordName);
    if (pRecord)
    {
        return pRecord->clear();
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strRecordName + "| There is no record", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const int64 nValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordInt(strRecordName, nRow, nCol, nValue))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error for row or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strRecordName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64 value)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordInt(strRecordName, nRow, strColTag, value))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error for row or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, strRecordName + "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordFloat(strRecordName, nRow, nCol, dwValue))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordFloat for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordFloat(strRecordName, nRow, strColTag, value))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordFloat for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordString(strRecordName, nRow, nCol, strValue))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordString for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordString(strRecordName, nRow, strColTag, value))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordObject for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol, const Guid& objectValue)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordObject(strRecordName, nRow, nCol, objectValue))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordObject for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool KernelModule::setRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const Guid& value)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        if (!pObject->setRecordObject(strRecordName, nRow, strColTag, value))
        {
            logModule_->normal(ILogModule::NLL_ERROR_NORMAL, self, strRecordName, "error setRecordObject for row  or col", __FUNCTION__, __LINE__);
        }
        else
        {
            return true;
        }
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return false;
}

int64 KernelModule::getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordInt(strRecordName, nRow, nCol);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return 0;
}

int64 KernelModule::getRecordInt(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordInt(strRecordName, nRow, strColTag);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return 0;
}

double KernelModule::getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordFloat(strRecordName, nRow, nCol);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return 0.0;
}

double KernelModule::getRecordFloat(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordFloat(strRecordName, nRow, strColTag);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return 0.0;
}

const std::string& KernelModule::getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordString(strRecordName, nRow, nCol);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return NULL_STR;
}

const std::string& KernelModule::getRecordString(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordString(strRecordName, nRow, strColTag);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object", __FUNCTION__, __LINE__);

    return NULL_STR;
}

const Guid& KernelModule::getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordObject(strRecordName, nRow, nCol);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object",  __FUNCTION__, __LINE__);

    return NULL_OBJECT;
}

const Guid& KernelModule::getRecordObject(const Guid& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    std::shared_ptr<IObject> pObject = getElement(self);
    if (pObject)
    {
        return pObject->getRecordObject(strRecordName, nRow, strColTag);
    }

    logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, self, "There is no object",  __FUNCTION__, __LINE__);

    return NULL_OBJECT;
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

int KernelModule::getMaxOnLineCount()
{
    // test count 5000
    // and it should be define in a xml file

    return 10000;
}

bool KernelModule::logStack()
{
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
#endif

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
#endif // ZQ_PLATFORM

    return true;
}

bool KernelModule::info(const Guid ident)
{  
    std::shared_ptr<IObject> pObject = getObject(ident);
    if (pObject)
    {
		int64 nSceneID = getPropertyInt(ident, config::Object::scene_id());
		//int nGroupID = getPropertyInt(ident, config::Object::group_id());

        logModule_->normal(ILogModule::NLL_INFO_NORMAL, ident, "//----------child object list-------- SceneID = ", nSceneID);

        DataList valObjectList;
		//GetGroupObjectList(nSceneID, nGroupID, valObjectList);
        for (int i  = 0; i < valObjectList.GetCount(); i++)
        {
           Guid targetIdent = valObjectList.Object(i);
           info(targetIdent);
       }
    }
    else
    {
        logModule_->logObject(ILogModule::NLL_ERROR_NORMAL, ident, "", __FUNCTION__, __LINE__);
    }

    return true;
}

int KernelModule::onPropertyCommonEvent(const Guid& self, const std::string& strPropertyName, const AbstractData& oldVar, const AbstractData& newVar)
{
	std::shared_ptr<IObject> xObject = getElement(self);
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
	}
	
    return 0;
}

std::shared_ptr<IObject> KernelModule::getObject(const Guid& ident)
{
    return getElement(ident);
}

int KernelModule::getObjectByProperty(const int nSceneID, const int nGroupID, const std::string& strPropertyName, const DataList& valueArg, DataList& list)
{
    DataList varObjectList;
	//GetGroupObjectList(nSceneID, nGroupID, varObjectList);

    int nWorldCount = varObjectList.GetCount();
    for (int i = 0; i < nWorldCount; i++)
    {
        Guid ident = varObjectList.Object(i);
        if (this->findProperty(ident, strPropertyName))
        {
            EN_DATA_TYPE eType = valueArg.Type(0);
            switch (eType)
            {
                case TDATA_INT:
                {
                    int64 nValue = getPropertyInt(ident, strPropertyName.c_str());
                    if (valueArg.Int(0) == nValue)
                    {
                        list.add(ident);
                    }
                }
                break;
                case TDATA_STRING:
                {
                    std::string strValue = getPropertyString(ident, strPropertyName.c_str());
                    std::string strCompareValue = valueArg.String(0);
                    if (strValue == strCompareValue)
                    {
                        list.add(ident);
                    }
                }
                break;
                case TDATA_OBJECT:
                {
                    Guid identObject = getPropertyObject(ident, strPropertyName.c_str());
                    if (valueArg.Object(0) == identObject)
                    {
                        list.add(ident);
                    }
                }
                break;
                default:
                    break;
            }
        }
    }

    return list.GetCount();
}

bool KernelModule::existObject(const Guid & ident)
{
	return existElement(ident);
}

bool KernelModule::destroySelf(const Guid& self)
{
    mtDeleteSelfList.push_back(self);
    return true;
}

int KernelModule::onRecordCommonEvent(const Guid& self, const RECORD_EVENT_DATA& xEventData, const AbstractData& oldVar, const AbstractData& newVar)
{
	std::shared_ptr<IObject> xObject = getElement(self);
	if (xObject)
	{
		if (xObject->getState() >= CLASS_OBJECT_EVENT::COE_CREATE_HASDATA)
		{
			for (auto it = mtCommonRecordCallBackList.begin(); it != mtCommonRecordCallBackList.end(); it++)
			{
				(*it)(self, xEventData, oldVar, newVar);
			}
		}

		const std::string& strClassName = xObject->getPropertyString(config::Object::class_name());
		auto itClass = mtClassRecordCallBackList.find(strClassName);
		if (itClass != mtClassRecordCallBackList.end())
		{
			for (auto itList = itClass->second.begin(); itList != itClass->second.end(); itList++)
			{
				(*itList)(self, xEventData, oldVar, newVar);
			}
		}

	}

    return 0;
}

int KernelModule::onClassCommonEvent(const Guid& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const DataList& var)
{
    for (auto it = mtCommonClassCallBackList.begin(); it != mtCommonClassCallBackList.end(); it++)
    {
        (*it)(self, strClassName, eClassEvent, var);
    }

    return 0;
}

bool KernelModule::registerCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb)
{
    mtCommonClassCallBackList.push_back(cb);
    return true;
}

bool KernelModule::registerCommonPropertyEvent(PROPERTY_EVENT_FUNCTOR&& cb)
{
    mtCommonPropertyCallBackList.push_back(cb);
    return true;
}

bool KernelModule::registerCommonRecordEvent(RECORD_EVENT_FUNCTOR&& cb)
{
    mtCommonRecordCallBackList.push_back(cb);
    return true;
}

bool KernelModule::registerClassPropertyEvent(const std::string& strClassName, PROPERTY_EVENT_FUNCTOR&& cb)
{
	mtClassPropertyCallBackList[strClassName].emplace_back(std::move(cb));
	return false;
}

bool KernelModule::registerClassRecordEvent(const std::string& strClassName, RECORD_EVENT_FUNCTOR&& cb)
{
	mtClassRecordCallBackList[strClassName].emplace_back(std::move(cb));
	return true;
}

bool KernelModule::logSelfInfo(const Guid ident)
{
    return false;
}

bool KernelModule::initEnd()
{
	using namespace std::placeholders;

    std::shared_ptr<IClass> pClass = classModule_->first();
    while (pClass)
    {
        registerClassCallBack(pClass->getClassName(), std::bind(&KernelModule::onClassCommonEvent, this, _1, _2, _3, _4));

        pClass = classModule_->next();
    }

    return true;
}

bool KernelModule::destroyAll()
{
    std::shared_ptr<IObject> pObject = first();
    while (pObject)
    {
        mtDeleteSelfList.push_back(pObject->self());

        pObject = next();
    }
  
    run();

    return true;
}

bool KernelModule::beforeShut()
{
    destroyAll();

	vecRandom_.clear();
	mtCommonClassCallBackList.clear();
	mtCommonPropertyCallBackList.clear();
	mtCommonRecordCallBackList.clear();

	mtClassPropertyCallBackList.clear();
	mtClassRecordCallBackList.clear();

    return true;
}

void KernelModule::random(int nStart, int nEnd, int nCount, DataList& valueList)
{
    if (randomPos_ + nCount >= (int)vecRandom_.size())
    {
        randomPos_ = 0;
    }

    for (int i = randomPos_; i < randomPos_ + nCount; i++)
    {
        float fRanValue = vecRandom_.at(i);
        int nValue = int((nEnd - nStart) * fRanValue) + nStart;
        valueList.add((int64)nValue);
    }

    randomPos_ += nCount;
}

int KernelModule::random(int nStart, int nEnd)
{
	if (randomPos_ + 1 >= (int)vecRandom_.size())
	{
		randomPos_ = 0;
	}

	float fRanValue = vecRandom_.at(randomPos_);
	randomPos_++;

	int nValue = int((nEnd - nStart) * fRanValue) + nStart;
	return nValue;
}

float KernelModule::random()
{
	randomPos_++;

	if (randomPos_ + 1 >= (int)vecRandom_.size())
	{
		randomPos_ = 0;
	}
	return vecRandom_.at(randomPos_);;
}

bool KernelModule::registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb)
{
    return classModule_->registerClassCallBack(strClassName, std::move(cb));
}

void KernelModule::processMemFree()
{
    if (lastActTime_ + 30 > libManager_->getNowTime())
    {
        return;
    }

    lastActTime_ = libManager_->getNowTime();
}

bool KernelModule::doEvent(const Guid& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const DataList& valueList)
{
    return classModule_->doEvent(self, strClassName, eEvent, valueList);
}
}