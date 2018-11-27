#include "class_module.h"
#include "baselib/base_code/util.h"
#include "baselib/core/object.h"
#include "dependencies/RapidXML/rapidxml.hpp"
#include "dependencies/RapidXML/rapidxml_print.hpp"

#include <time.h>
#include <algorithm>
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <conio.h>
#else
#include <iconv.h>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#endif


namespace zq{



static std::vector<std::string> getFileListInFolder(const std::string& folderPath, int depth)
{
	std::vector<std::string> result;

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
	_finddata_t FileInfo;
	std::string strfind = folderPath + "\\*";
	long long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L)
	{
		std::cerr << "can not match the folder path" << std::endl;
		assert(false);
	}
	do {

		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				std::string newPath = folderPath + "\\" + FileInfo.name;
				//dfsFolder(newPath, depth);
			}
		}
		else
		{

			std::string filename = (folderPath + "\\" + FileInfo.name);
			result.push_back(filename);
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);

#else
	DIR *pDir;
	struct dirent *ent;
	char childpath[512];
	char absolutepath[512];
	pDir = opendir(folderPath.c_str());
	memset(childpath, 0, sizeof(childpath));
	while ((ent = readdir(pDir)) != NULL)
	{
		if (ent->d_type & DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
		}
		else
		{
			sprintf(absolutepath, "%s/%s", folderPath.c_str(), ent->d_name);
			result.push_back(absolutepath);
		}
	}

	std::sort(result.begin(), result.end());
#endif

	return result;
}

CClassModule::CClassModule(ILibManager* p)
{
    libManager_ = p;
	strXmlDir_ = libManager_->getCfgXmlDir();
	strDBXmlDir_ = "../datacfg/db_xml/";
}

CClassModule::~CClassModule()
{
}

bool CClassModule::init()
{
	kernalModule_ = libManager_->findModule<IKernelModule>();
	//load();
	return true;
}

bool CClassModule::shut()
{
	return true;
}

static inline EN_DATA_TYPE computerType(const std::string& pstrTypeName)
{
    if (pstrTypeName == "int")
    {
        return TDATA_INT64;
    }
	if (pstrTypeName == "string")
    {
		return TDATA_STRING;
    }
	if (pstrTypeName == "float")
    {
		return TDATA_DOUBLE;
    }

	return TDATA_UNKNOWN;
}

bool CClassModule::addClass(const std::string& strClassName, CClassPtr newclass)
{
	return classList_.insert(std::make_pair(strClassName, newclass)).second;
}

bool CClassModule::load()
{
	// 先获得这个路径下的所有文件
	//auto fileList = getFileListInFolder(strXmlDir_, 0);
	auto fileList = getFileListInFolder(strDBXmlDir_, 0);
	for (auto fileName : fileList)
	{
		util::strReplace(fileName, "\\", "/");
		util::strReplace(fileName, "//", "/");

		if (fileName.find(".xml") == std::string::npos)
		{
			std::cout << "find error file, file: " << fileName << std::endl;
			continue;
		}

		// 开始读取
		if (!readDBXml(fileName))
		{
			std::cout << "Create " + fileName + " failed!" << std::endl;
			return false;
		}
	}

    return true;
}

bool CClassModule::readXml(const std::string& file_name)
{
	//// 把文件名作为类名
	//auto claa_name = file_name.substr(file_name.find_last_of('/') + 1, file_name.find_last_of('.') - file_name.find_last_of('/') - 1);
	//std::shared_ptr<CClass> pClass(new CClass(claa_name));

	//std::string strContent;
	//libManager_->getFileContent(file_name, strContent);
	//rapidxml::xml_document<> xDoc;
	//xDoc.parse<0>((char*)strContent.c_str());

	//// 属性的描述
	//rapidxml::xml_node<>* property_desc_node = xDoc.first_node("property_desc");
	//for (auto property_node = property_desc_node->first_node(); property_node; property_node = property_node->next_sibling())
	//{
	//	std::string property_name = property_node->name();
	//	for (auto attr = property_node->first_attribute(); attr; attr = attr->next_attribute())
	//	{
	//		std::string str_type = property_node->first_attribute("type")->value();

	//		// 转换类型
	//		EN_DATA_TYPE data_type = computerType(str_type);
	//		if (TDATA_UNKNOWN == data_type)
	//		{
	//			ASSERT(false, "TDATA_UNKNOWN == computerType");
	//			return false;
	//		}

	//		// 把这个属性添加进属性管理器，这里只是添加了一个属性，还没有对这个属性进行赋值
	//		pClass->getDescPropertyMgr().newProperty(property_name, data_type);
	//	}
	//}

	//// 属性的值
	//rapidxml::xml_node<>* class_node = xDoc.first_node(claa_name.c_str());
	//for (auto obj_node = class_node->first_node(); obj_node; obj_node = obj_node->next_sibling())
	//{
	//	std::string obj_name = obj_node->name();
	//	std::shared_ptr<CObject> new_obj = std::make_shared<CObject>();
	//	new_obj->setObjName(obj_name);

	//	for (auto attr = obj_node->first_attribute(); attr; attr = attr->next_attribute())
	//	{
	//		const char* attr_name = attr->name();
	//		const char* attr_value = attr->value();

	//		// 是否存在这个属性的描述，这里肯定是存在的，不然就是配置错误
	//		auto property_desc = pClass->getDescPropertyMgr().getProperty(attr_name);
	//		if (property_desc == nullptr)
	//		{
	//			ASSERT(false);
	//			return false;
	//		}

	//		// 这里再把objct的属性名字判断一次，防止重复
	//		if (new_obj->exsitProperty(attr_name))
	//		{
	//			ASSERT(false);
	//			return false;
	//		}

	//		// 开始赋值
	//		VariantData var;
	//		const EN_DATA_TYPE eType = property_desc->getType();
	//		try
	//		{
	//			switch (eType)
	//			{
	//			case TDATA_INT64:
	//			{
	//				new_obj->setValue(attr_name, std::stoll(attr_value));
	//			}
	//			break;
	//			case TDATA_DOUBLE:
	//			{
	//				new_obj->setValue(attr_name, std::stod(attr_value));
	//			}
	//			break;
	//			case TDATA_STRING:
	//			{
	//				new_obj->setValue(attr_name, attr_value);
	//			}
	//			break;
	//			case TDATA_GUID:
	//			{
	//				new_obj->setValue(attr_name, Guid());
	//			}
	//			break;
	//			default:
	//				ASSERT(false);
	//				break;
	//			}
	//		}
	//		catch (const std::exception& e)
	//		{
	//			ASSERT(false, e.what());
	//			return false;
	//		}
	//	}

	//	//添加该类的一个对象
	//	bool b = pClass->addStaticObj(obj_name, new_obj);
	//	ASSERT(b == true);
	//}

	//if (!addClass(claa_name, pClass))
	//{
	//	ASSERT(false);
	//	return false;
	//}

	return true;
}

bool CClassModule::readDBXml(const std::string& file_name)
{
	std::string strContent;
	libManager_->getFileContent(file_name, strContent);
	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

	rapidxml::xml_node<>* root = xDoc.first_node();
	std::string class_name = root->name();
											
	// 先把所有配置的类名找出来，防止重复
	rapidxml::xml_node<>* class_node = root->first_node("db_data");
	std::map<std::string, bool> vec_extern_name;
	for (auto extern_node = class_node->first_node(); extern_node; extern_node = extern_node->next_sibling())
	{
		std::string extern_name = extern_node->name();
		if (!vec_extern_name.insert(std::make_pair(extern_name, true)).second)
		{
			ASSERT(false);
			return false;
		}
	}

	class_node = root->first_node("class_extern");
	for (auto extern_node = class_node->first_node(); extern_node; extern_node = extern_node->next_sibling())
	{
		std::string extern_name = extern_node->name();
		if (!vec_extern_name.insert(std::make_pair(extern_name, true)).second)
		{
			ASSERT(false);
			return false;
		}
	}

	for (auto root_child_node = root->first_node(); root_child_node; root_child_node = root_child_node->next_sibling())
	{
		for (auto extern_node = root_child_node->first_node(); extern_node; extern_node = extern_node->next_sibling())
		{
			std::string claa_name = extern_node->name();
			std::shared_ptr<CClass> pClass(new CClass(claa_name));

			for (auto child_node = extern_node->first_node(); child_node; child_node = child_node->next_sibling())
			{
				std::string name = child_node->name();
				std::string str_type = child_node->first_attribute("type")->value();

				// 转换类型
				EN_DATA_TYPE data_type = computerType(str_type);
				if (TDATA_UNKNOWN == data_type)
				{
					// 如果这里基本类型找不到，就去看下是否是自定义的类
					auto it = vec_extern_name.find(str_type);
					if (it == vec_extern_name.end())
					{
						ASSERT(false, "TDATA_UNKNOWN == computerType");
						return false;
					}

					pClass->addMemberClassName(str_type);
				}

				// 把这个属性添加进属性管理器，这里只是添加了一个属性，还没有对这个属性进行赋值
				PropertyPtr xProperty = pClass->getDescPropertyMgr().newProperty(name, data_type);
			}

			if (!addClass(claa_name, pClass))
			{
				ASSERT(false);
				return false;
			}
		}
	}

	for (auto it = classList_.begin(); it != classList_.end(); ++it)
	{
		CClassPtr cclass = it->second;
		for (const auto& obj : cclass->getAllMemObjs())
		{

		}
	}

	return true;
}

std::shared_ptr<IClass> CClassModule::getClass(const std::string& class_anme)
{
	auto it = classList_.find(class_anme);
	if (it == classList_.end())
	{
		return nullptr;
	}

	return it->second;
}

CClassPtr CClassModule::findClass(const std::string& class_anme)
{
	auto it = classList_.find(class_anme);
	if (it == classList_.end())
	{
		return nullptr;
	}

	return it->second;
}

IObjectPtr CClassModule::createObject(const std::string& class_name)
{
	using namespace std::placeholders;

	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return nullptr;
	}

	// 这里应该不可能存在，不然这个函数就出大问题了
	Guid ident = kernalModule_->createGUID();
	if (cclass->getMemObj(ident) != nullptr)
	{
		LOG_ERROR << "The object has Exists, id: " << ident.toString();
		return nullptr;
	}

	IObjectPtr pObject = std::make_shared<CObject>();
	cclass->addMemObj(pObject);

	return pObject;
}

bool CClassModule::destroyObject(const std::string& class_name, const Guid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass == nullptr, class_name: " << class_name;
		return false;
	}

	return cclass->removeMemObj(ident);
}

IObjectPtr CClassModule::getObject(const std::string& class_name, const Guid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return nullptr;
	}

	return cclass->getMemObj(ident);
}

bool CClassModule::existObject(const std::string& class_name, const Guid& ident)
{
	CClassPtr cclass = findClass(class_name);
	if (cclass == nullptr)
	{
		LOG_ERROR << "iclass==nullptr, class_name: " << class_name;
		return false;
	}

	return cclass->getMemObj(ident) != nullptr;
}


}
