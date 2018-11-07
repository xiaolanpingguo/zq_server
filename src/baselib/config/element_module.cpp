#include <algorithm>
#include <ctype.h>
#include "config.h"
#include "element_module.h"
#include "class_module.h"
using namespace zq;


constexpr const char* XML_CLASS_NAME = "class_name";
static const std::string DATACFG_DIR_NAME = "../datacfg/";
static const std::string STRUCT_DIR_NAME = "struct/";
static const std::string LOGIC_CLASS_FILE_NAME = "LogicClass.xml";

ElementModule::ElementModule(ILibManager* p)
{
    libManager_ = p;
    loaded_ = false;
}

ElementModule::~ElementModule()
{

}

bool ElementModule::init()
{
    classModule_ = libManager_->findModule<IClassModule>();
	
	load();

    return true;
}

bool ElementModule::shut()
{
    clear();
	loaded_ = false;
    return true;
}

bool ElementModule::load()
{
    if (loaded_)
    {
        return false;
    }

    std::shared_ptr<IClass> pLogicClass = classModule_->first();
    while (pLogicClass)
    {
        const std::string& strInstancePath = pLogicClass->getInstancePath();
        if (strInstancePath.empty())
        {
            pLogicClass = classModule_->next();
            continue;
        }

		// 解析实现文件(impl)，相当于这个类的所有实例
		std::string strFile = DATACFG_DIR_NAME + strInstancePath;
		std::string strContent;
		libManager_->getFileContent(strFile, strContent);

		std::cout << "load impl file: " << strFile << std::endl;

		rapidxml::xml_document<> xDoc;
		xDoc.parse<0>((char*)strContent.c_str());

        rapidxml::xml_node<>* root = xDoc.first_node();
        for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
        {
            load(attrNode, pLogicClass);
        }

        loaded_ = true;
        pLogicClass = classModule_->next();
    }

    return true;
}

bool ElementModule::CheckRef()
{
    std::shared_ptr<IClass> pLogicClass = classModule_->first();
    while (pLogicClass)
    {
		std::shared_ptr<IPropertyManager> pClassPropertyManager = pLogicClass->getPropertyManager();
		if (pClassPropertyManager)
		{
			std::shared_ptr<IProperty> pProperty = pClassPropertyManager->first();
			while (pProperty)
			{
				//if one property is ref,check every config
				if (pProperty->getRef())
				{
					const std::vector<std::string>& strIdList = pLogicClass->getIDList();
					for (size_t i = 0; i < strIdList.size(); ++i)
					{
						const std::string& strId = strIdList[i];

						const std::string& strRefValue= this->getPropertyString(strId, pProperty->getKey());
						if (!strRefValue.empty() && !this->getElement(strRefValue))
						{
							std::string msg;
							msg.append("check ref failed id: ").append(strRefValue).append(" in ").append(pLogicClass->getClassName());
							ASSERT(false);
							exit(0);
						}
					}
				}
				pProperty = pClassPropertyManager->next();
			}
		}
        //////////////////////////////////////////////////////////////////////////
        pLogicClass = classModule_->next();
    }

    return false;
}

bool ElementModule::load(rapidxml::xml_node<>* attrNode, std::shared_ptr<IClass> pLogicClass)
{
    // 实例化的类名,比如gameserver1,gameserver2
    std::string strConfigID = attrNode->first_attribute("Id")->value();
    if (strConfigID.empty())
    {
        ASSERT(false);
        return false;
    }

	// 是否重复了
    if (existElement(strConfigID))
    {
        ASSERT(false);
        return false;
    }

	// 添加一个类
    std::shared_ptr<ElementConfigInfo> pElementInfo(new ElementConfigInfo());
    addElement(strConfigID, pElementInfo);

    // 添加这个类的实例的名字
    pLogicClass->addId(strConfigID);

    std::shared_ptr<IPropertyManager> pElementPropertyManager = pElementInfo->getPropertyManager();
    std::shared_ptr<IRecordManager> pElementRecordManager = pElementInfo->getRecordManager();

	// 属性的描述
    std::shared_ptr<IPropertyManager> pClassPropertyManager = pLogicClass->getPropertyManager();
    std::shared_ptr<IRecordManager> pClassRecordManager = pLogicClass->getRecordManager();
    if (pClassPropertyManager && pClassRecordManager)
    {
		// 基本属性，每一个属性都有一个唯一的id
        std::shared_ptr<IProperty> pProperty = pClassPropertyManager->first();
        while (pProperty)
        {
            pElementPropertyManager->addProperty(Guid(), pProperty);

            pProperty = pClassPropertyManager->next();
        }

		// 多维属性
        std::shared_ptr<IRecord> pRecord = pClassRecordManager->first();
        while (pRecord)
        {
            std::shared_ptr<IRecord> xRecord = pElementRecordManager->AddRecord(Guid(), pRecord->getName(), pRecord->getInitData(), pRecord->getTag(), pRecord->getRows());

            xRecord->setPublic(pRecord->getPublic());
            xRecord->setPrivate(pRecord->getPrivate());
            xRecord->setSave(pRecord->getSave());
            xRecord->setCache(pRecord->getCache());
			xRecord->setRef(pRecord->getRef());
			xRecord->setForce(pRecord->getForce());
			xRecord->setUpload(pRecord->getUpload());

            pRecord = pClassRecordManager->next();
        }

    }

	// 属性的值
    for (rapidxml::xml_attribute<>* pAttribute = attrNode->first_attribute(); pAttribute; pAttribute = pAttribute->next_attribute())
    {
        const char* pstrConfigName = pAttribute->name();
        const char* pstrConfigValue = pAttribute->value();

        std::shared_ptr<IProperty> temProperty = pElementPropertyManager->getElement(pstrConfigName);
        if (!temProperty)
        {
            continue;
        }

        AbstractData var;
        const EN_DATA_TYPE eType = temProperty->getType();
        switch (eType)
        {
            case TDATA_INT:
            {
                if (!LegalNumber(pstrConfigValue))
                {
                    ASSERT(false);
                }
                var.setInt(lexical_cast<int64>(pstrConfigValue));
            }
            break;
            case TDATA_FLOAT:
            {
                if (strlen(pstrConfigValue) <= 0)
                {
                    ASSERT(false);
                }
                var.setFloat((double)atof(pstrConfigValue));
            }
            break;
            case TDATA_STRING:
                {
                    var.setString(pstrConfigValue);
                }
                break;
            case TDATA_OBJECT:
            {
                if (strlen(pstrConfigValue) <= 0)
                {
                    ASSERT(false);
                }
                var.setObject(Guid());
            }
            break;
            default:
                ASSERT(false);
                break;
        }

        temProperty->setValue(var);
        if (eType == TDATA_STRING)
        {
            temProperty->deSerialization();
        }
    }

    AbstractData xData;
    xData.setString(pLogicClass->getClassName());
    pElementPropertyManager->setProperty(XML_CLASS_NAME, xData);

    return true;
}

bool ElementModule::save()
{
    return true;
}

int64 ElementModule::getPropertyInt(const std::string& strConfigName, const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if (pProperty)
    {
        return pProperty->getInt();
    }

    return 0;
}

double ElementModule::getPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if (pProperty)
    {
        return pProperty->getFloat();
    }

    return 0.0;
}

const std::string& ElementModule::getPropertyString(const std::string& strConfigName, const std::string& strPropertyName)
{
    std::shared_ptr<IProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if (pProperty)
    {
        return pProperty->getString();
    }

    return  NULL_STR;
}

const std::vector<std::string> ElementModule::getListByProperty(const std::string & strClassName, const std::string & strPropertyName, int64 nValue)
{
	std::vector<std::string> xList;

	std::shared_ptr<IClass> xClass = classModule_->getElement(strClassName);
	if (nullptr != xClass)
	{
		const std::vector<std::string>& xElementList = xClass->getIDList();
		for (size_t i = 0; i < xElementList.size(); ++i)
		{
			const std::string& strConfigID = xElementList[i];
			int64 nElementValue = getPropertyInt(strConfigID, strPropertyName);
			if (nValue == nElementValue)
			{
				xList.push_back(strConfigID);
			}
		}
	}

	return xList;
}

const std::vector<std::string> ElementModule::getListByProperty(const std::string & strClassName, const std::string & strPropertyName, const std::string & nValue)
{
	std::vector<std::string> xList;

	std::shared_ptr<IClass> xClass = classModule_->getElement(strClassName);
	if (nullptr != xClass)
	{
		const std::vector<std::string>& xElementList = xClass->getIDList();
		for (size_t i = 0; i < xElementList.size(); ++i)
		{
			const std::string& strConfigID = xElementList[i];
			const std::string& strElementValue = getPropertyString(strConfigID, strPropertyName);
			if (nValue == strElementValue)
			{
				xList.push_back(strConfigID);
			}
		}
	}

	return xList;
}

std::shared_ptr<IProperty> ElementModule::GetProperty(const std::string& strConfigName, const std::string& strPropertyName)
{
    std::shared_ptr<ElementConfigInfo> pElementInfo = getElement(strConfigName);
    if (pElementInfo)
    {
        return pElementInfo->getPropertyManager()->getElement(strPropertyName);
    }

    return NULL;
}

std::shared_ptr<IPropertyManager> ElementModule::getPropertyManager(const std::string& strConfigName)
{
    std::shared_ptr<ElementConfigInfo> pElementInfo = getElement(strConfigName);
    if (pElementInfo)
    {
        return pElementInfo->getPropertyManager();
    }

    return NULL;
}

std::shared_ptr<IRecordManager> ElementModule::getRecordManager(const std::string& strConfigName)
{
    std::shared_ptr<ElementConfigInfo> pElementInfo = getElement(strConfigName);
    if (pElementInfo)
    {
        return pElementInfo->getRecordManager();
    }
    return NULL;
}

bool ElementModule::loadSceneInfo(const std::string& strFileName, const std::string& strClassName)
{
	std::string strContent;
	libManager_->getFileContent(strFileName, strContent);

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());
	
    std::shared_ptr<IClass> pLogicClass = classModule_->getElement(strClassName.c_str());
    if (pLogicClass)
    {
        //support for unlimited layer class inherits
        rapidxml::xml_node<>* root = xDoc.first_node();
        for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
        {
            load(attrNode, pLogicClass);
        }
    }
    else
    {
        std::cout << "error load scene info failed, name is:" << strClassName << " file name is :" << strFileName << std::endl;
    }

    return true;
}

bool ElementModule::existElement(const std::string& strConfigName)
{
    std::shared_ptr<ElementConfigInfo> pElementInfo = getElement(strConfigName);
    if (pElementInfo)
    {
        return true;
    }

    return false;
}

bool ElementModule::existElement(const std::string& strClassName, const std::string& strConfigName)
{
    std::shared_ptr<ElementConfigInfo> pElementInfo = getElement(strConfigName);
    if (!pElementInfo)
    {
        return false;
    }

    const std::string& strClass = pElementInfo->getPropertyManager()->getPropertyString(XML_CLASS_NAME);
    if (strClass != strClassName)
    {
        return false;
    }

    return true;
}

bool ElementModule::LegalNumber(const char* str)
{
    int nLen = int(strlen(str));
    if (nLen <= 0)
    {
        return false;
    }

    int nStart = 0;
    if ('-' == str[0])
    {
        nStart = 1;
    }

    for (int i = nStart; i < nLen; ++i)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }

    return true;
}

bool ElementModule::initEnd()
{
    CheckRef();
    return true;

}

bool ElementModule::beforeShut()
{
    return true;

}

bool ElementModule::run()
{
    return true;
}

bool ElementModule::reload(const std::string& filename)
{
	// 去掉后缀名
	std::string name = filename.substr(0, filename.find_last_of('.'));

	// 查找这个class
	std::shared_ptr<IClass> pLogicClass = classModule_->getElement(name);
	if (pLogicClass == nullptr)
	{
		return false;
	}

	// impl的配置路径
	const std::string& strInstancePath = pLogicClass->getInstancePath();
	if (strInstancePath.empty())
	{
		return false;
	}

	// 清空之前这个类的所有实例
	const auto& id_list = pLogicClass->getIDList();
	for (size_t i = 0; i < id_list.size(); ++i)
	{
		removeElement(id_list[i]);
	}
	pLogicClass->clearIdList();

	// 重新读取文件
	std::string strFile = DATACFG_DIR_NAME + strInstancePath;
	std::string strContent;
	libManager_->getFileContent(strFile, strContent);

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

	rapidxml::xml_node<>* root = xDoc.first_node();
	for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
	{
		load(attrNode, pLogicClass);
	}

	return true;
}