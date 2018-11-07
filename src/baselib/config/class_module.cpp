#include <time.h>
#include <algorithm>
#include "config.h"
#include "class_module.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "Dependencies/RapidXML/rapidxml_print.hpp"

using namespace zq;


static const std::string LOGIC_CLASS_FILE_NAME = "LogicClass.xml";

CClassModule::CClassModule(ILibManager* p)
{
    libManager_ = p;
	cfgRootPath_ = libManager_->getCfgRootDir();
	structPath_ = libManager_->getCfgStructDir();
	implPath_ = libManager_->getCfgImplDir();

    logicClassConfigName_ = structPath_ + LOGIC_CLASS_FILE_NAME;
}

CClassModule::~CClassModule()
{
    clear();
}

bool CClassModule::init()
{
	elementModule_ = libManager_->findModule<IElementModule>();

	load();

	return true;
}

bool CClassModule::shut()
{
	clear();

	return true;
}

EN_DATA_TYPE CClassModule::computerType(const char* pstrTypeName, AbstractData& var)
{
    if (0 == strcmp(pstrTypeName, "int"))
    {
        var.setInt(NULL_INT);
        return var.getType();
    }
    else if (0 == strcmp(pstrTypeName, "string"))
    {
        var.setString(NULL_STR);
        return var.getType();
    }
    else if (0 == strcmp(pstrTypeName, "float"))
    {
        var.setFloat(NULL_FLOAT);
        return var.getType();
    }
    else if (0 == strcmp(pstrTypeName, "object"))
    {
        var.setObject(NULL_OBJECT);
        return var.getType();
    }

    return TDATA_UNKNOWN;
}

bool CClassModule::addPropertys(rapidxml::xml_node<>* pPropertyRootNode, std::shared_ptr<IClass> pClass)
{
    for (rapidxml::xml_node<>* pPropertyNode = pPropertyRootNode->first_node(); pPropertyNode; pPropertyNode = pPropertyNode->next_sibling())
    {
		// 属性名，可以类似为一个成员变量，这里当然就是成员变量的名字
        const char* strPropertyName = pPropertyNode->first_attribute("Id")->value();
        if (pClass->getPropertyManager()->getElement(strPropertyName))
        {
            ASSERT(0, strPropertyName, __FILE__, __FUNCTION__);
            continue;
        }

        const char* pstrType = pPropertyNode->first_attribute("Type")->value();
        const char* pstrPublic = pPropertyNode->first_attribute("Public")->value();
        const char* pstrPrivate = pPropertyNode->first_attribute("Private")->value();
        const char* pstrSave = pPropertyNode->first_attribute("Save")->value();
        const char* pstrCache = pPropertyNode->first_attribute("Cache")->value();
		const char* pstrRef = pPropertyNode->first_attribute("Ref")->value();
		const char* pstrForce = pPropertyNode->first_attribute("Force")->value();
		const char* pstrUpload = pPropertyNode->first_attribute("Upload")->value();

		// 属性的其他描述，是否公开，是否存档等
        bool bPublic = lexical_cast<bool>(pstrPublic);
        bool bPrivate = lexical_cast<bool>(pstrPrivate);
        bool bSave = lexical_cast<bool>(pstrSave);
        bool bCache = lexical_cast<bool>(pstrCache);
		bool bRef = lexical_cast<bool>(pstrRef);
		bool bForce = lexical_cast<bool>(pstrForce);
		bool bUpload = lexical_cast<bool>(pstrUpload);

		// 把配置中的int,string等转换为抽象的属性，对应这里就是变量的类型
        AbstractData varProperty;
        if (TDATA_UNKNOWN == computerType(pstrType, varProperty))
        {
            ASSERT(false, strPropertyName, __FILE__, __FUNCTION__);
        }

		// 把这个属性添加进属性管理器，属性名--变量名，属性类型--变量类型，属性其他--是否存在，是否是public等
		// 这里只是添加了一个属性，还没有对这个属性进行赋值
        std::shared_ptr<IProperty> xProperty = pClass->getPropertyManager()->addProperty(Guid(), strPropertyName, varProperty.getType());
        xProperty->setPublic(bPublic);
        xProperty->setPrivate(bPrivate);
        xProperty->setSave(bSave);
        xProperty->setCache(bCache);
		xProperty->setRef(bRef);
		xProperty->setForce(bForce);
		xProperty->setUpload(bUpload);
    }

    return true;
}

bool CClassModule::addRecords(rapidxml::xml_node<>* pRecordRootNode, std::shared_ptr<IClass> pClass)
{
    for (rapidxml::xml_node<>* pRecordNode = pRecordRootNode->first_node(); pRecordNode;  pRecordNode = pRecordNode->next_sibling())
    {
        const char* pstrRecordName = pRecordNode->first_attribute("Id")->value();

        if (pClass->getRecordManager()->getElement(pstrRecordName))
        {
            ASSERT(0, pstrRecordName, __FILE__, __FUNCTION__);
            continue;
        }

        const char* pstrRow = pRecordNode->first_attribute("Row")->value();
        //const char* pstrCol = pRecordNode->first_attribute("Col")->value();
        const char* pstrPublic = pRecordNode->first_attribute("Public")->value();
        const char* pstrPrivate = pRecordNode->first_attribute("Private")->value();
        const char* pstrSave = pRecordNode->first_attribute("Save")->value();
		const char* pstrCache = pRecordNode->first_attribute("Cache")->value();
		//const char* pstrRef = pRecordNode->first_attribute("Ref")->value();
		//const char* pstrForce = pRecordNode->first_attribute("Force")->value();
		const char* pstrUpload = pRecordNode->first_attribute("Upload")->value();

        std::string strView;
        if (pRecordNode->first_attribute("View") != NULL)
        {
            strView = pRecordNode->first_attribute("View")->value();
        }

        bool bPublic = lexical_cast<bool>(pstrPublic);
        bool bPrivate = lexical_cast<bool>(pstrPrivate);
        bool bSave = lexical_cast<bool>(pstrSave);
		bool bCache = lexical_cast<bool>(pstrCache);
		bool bRef = lexical_cast<bool>(pstrCache);
		bool bForce = lexical_cast<bool>(pstrCache);
		bool bUpload = lexical_cast<bool>(pstrUpload);

		std::shared_ptr<DataList> recordVar(new DataList());
		std::shared_ptr<DataList> recordTag(new DataList());

        for (rapidxml::xml_node<>* recordColNode = pRecordNode->first_node(); recordColNode;  recordColNode = recordColNode->next_sibling())
        {
            //const char* pstrColName = recordColNode->first_attribute( "Id" )->value();
            AbstractData TData;
            const char* pstrColType = recordColNode->first_attribute("Type")->value();
            if (TDATA_UNKNOWN == computerType(pstrColType, TData))
            {
                //assert(0);
                ASSERT(0, pstrRecordName, __FILE__, __FUNCTION__);
            }

            recordVar->Append(TData);

            if (recordColNode->first_attribute("Tag") != NULL)
            {
                const char* pstrTag = recordColNode->first_attribute("Tag")->value();
                recordTag->add(pstrTag);
            }
            else
            {
                recordTag->add("");
            }
        }

        std::shared_ptr<IRecord> xRecord = pClass->getRecordManager()->AddRecord(Guid(), pstrRecordName, recordVar, recordTag, atoi(pstrRow));

		xRecord->setPublic(bPublic);
        xRecord->setPrivate(bPrivate);
        xRecord->setSave(bSave);
        xRecord->setCache(bCache);
		xRecord->setRef(bRef);
		xRecord->setForce(bForce);
		xRecord->setUpload(bUpload);
    }

    return true;
}

bool CClassModule::addClassInclude(const char* pstrClassFilePath, std::shared_ptr<IClass> pClass)
{
    if (pClass->find(pstrClassFilePath))
    {
        return false;
    }

	// 父类的路径, "../datacfg/" + "struct/*.xml"
    std::string strFile = cfgRootPath_ + pstrClassFilePath;												
	std::string strContent;
	libManager_->getFileContent(strFile, strContent);

	rapidxml::xml_document<> xDoc;
    xDoc.parse<0>((char*)strContent.c_str());

	// 基础属性
    rapidxml::xml_node<>* root = xDoc.first_node();
    rapidxml::xml_node<>* pRropertyRootNode = root->first_node("Propertys");
    if (pRropertyRootNode)
    {
        addPropertys(pRropertyRootNode, pClass);
    }

	// 多维数据
    rapidxml::xml_node<>* pRecordRootNode = root->first_node("Records");
    if (pRecordRootNode)
    {
        addRecords(pRecordRootNode, pClass);
    }

	// 包含文件(目前没用到)
    rapidxml::xml_node<>* pIncludeRootNode = root->first_node("Includes");
    if (pIncludeRootNode)
    {
        for (rapidxml::xml_node<>* includeNode = pIncludeRootNode->first_node(); includeNode; includeNode = includeNode->next_sibling())
        {
            const char* pstrIncludeFile = includeNode->first_attribute("Id")->value();
            //std::vector<std::string>::iterator it = std::find( pClass->mvIncludeFile.begin(), pClass->mvIncludeFile..end(), pstrIncludeFile );

            if (addClassInclude(pstrIncludeFile, pClass))
            {
                pClass->add(pstrIncludeFile);
            }
        }
    }

    return true;
}

bool CClassModule::addClass(const char* pstrClassFilePath, std::shared_ptr<IClass> pClass)
{
	// 有父类
    std::shared_ptr<IClass> pParent = pClass->getParent();
    while (pParent)
    {
		// 所有的父类，嵌套结构
        std::string strFileName = "";
        pParent->first(strFileName);
        while (!strFileName.empty())
        {
			// 子类已经存在父类的名字了
            if (pClass->find(strFileName))
            {
                strFileName.clear();
                continue;
            }

			// 读取父类的属性和record
            if (addClassInclude(strFileName.c_str(), pClass))
            {
				// 这个类已经包含了这个父类
                pClass->add(strFileName);
            }

            strFileName.clear();
            pParent->next(strFileName);
        }

        pParent = pParent->getParent();
    }

	// 读取本类的属性和record
    if (addClassInclude(pstrClassFilePath, pClass))
    {
        pClass->add(pstrClassFilePath);
    }

    return true;
}

bool CClassModule::addClass(const std::string& strClassName, const std::string& strParentName)
{
    std::shared_ptr<IClass> pParentClass = getElement(strParentName);
    std::shared_ptr<IClass> pChildClass = getElement(strClassName);
    if (!pChildClass)
    {
        pChildClass = std::shared_ptr<IClass>(new CClass(strClassName));
        addElement(strClassName, pChildClass);
        //pChildClass = CreateElement( strClassName );

        pChildClass->setTypeName("");
        pChildClass->setInstancePath("");

        if (pParentClass)
        {
            pChildClass->setParent(pParentClass);
        }
    }

    return true;
}

bool CClassModule::load(rapidxml::xml_node<>* attrNode, std::shared_ptr<IClass> pParentClass)
{
	// 类名
    const char* pstrLogicClassName = attrNode->first_attribute("Id")->value();

	// struct路径
    const char* pstrPath = attrNode->first_attribute("Path")->value();

	// impl路径
    const char* pstrInstancePath = attrNode->first_attribute("InstancePath")->value();

	// 添加元素,key为类名
    std::shared_ptr<IClass> pClass(new CClass(pstrLogicClassName));
	if (!addElement(pstrLogicClassName, pClass))
	{
		return false;
	}

	// 设置父类和impl的路径
    pClass->setParent(pParentClass);
    pClass->setInstancePath(pstrInstancePath);

	// 读取class属性
    addClass(pstrPath, pClass);

    for (rapidxml::xml_node<>* pDataNode = attrNode->first_node(); pDataNode; pDataNode = pDataNode->next_sibling())
    {
        // 子节点
        load(pDataNode, pClass);
    }

    return true;
}

bool CClassModule::load()
{
	// 加载LogicClass.xml，获得各个配置文件的路径
	std::string strFile = logicClassConfigName_;
	std::string strContent;
	libManager_->getFileContent(strFile, strContent);

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

    rapidxml::xml_node<>* root = xDoc.first_node();
    for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
    {
        load(attrNode, nullptr);
    }

    return true;
}

bool CClassModule::Save()
{
    return true;
}

std::shared_ptr<IPropertyManager> CClassModule::getClassPropertyManager(const std::string& strClassName)
{
    std::shared_ptr<IClass> pClass = getElement(strClassName);
    if (pClass)
    {
        return pClass->getPropertyManager();
    }

    return NULL;
}

std::shared_ptr<IRecordManager> CClassModule::getClassRecordManager(const std::string& strClassName)
{
    std::shared_ptr<IClass> pClass = getElement(strClassName);
    if (pClass)
    {
        return pClass->getRecordManager();
    }

    return NULL;
}

bool CClassModule::clear()
{
    return true;
}

bool CClassModule::registerClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb)
{
    std::shared_ptr<IClass> pClass = getElement(strClassName);
    if (nullptr == pClass)
    {
        return false;
    }

    return pClass->registerClassCallBack(std::move(cb));
}

bool CClassModule::doEvent(const Guid& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const DataList& valueList)
{
    std::shared_ptr<IClass> pClass = getElement(strClassName);
    if (nullptr == pClass)
    {
        return false;
    }

    return pClass->doEvent(objectID, eClassEvent, valueList);
}
