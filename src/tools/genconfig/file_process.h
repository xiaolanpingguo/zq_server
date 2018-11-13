#pragma once



#include "excel_reader.h"
#include <map>
#include <list>


static const std::string CPP_DIR = "cpp/";	
static const std::string XML_NAME = "xml/";

static const std::string BEGIN_NAMESPACE = "namespace zq\n{\nnamespace config\n{\n";
static const std::string END_NAMESPACE = "\n}}\n";
static const std::string THIS_NAME = "this_name";

static const std::string CPP_FILE_NAME = "config_define.hpp";

// 配置表的属性描述和对象的行分割标识
static const std::string STR_SEPARATOR = "separator##";

struct ClassData
{
	// 对该属性的描述，名字，类型，值
	struct ClassPropertyDescInfo
	{
		std::string property_name;

		struct DescInfo
		{
			std::string desc_name;
			std::string desc_value;
		};

		std::list<DescInfo> vec_desc_info; // 对该属性的所有描述
	};

	struct ObjectInfo
	{
		// 类中的属性信息
		struct PropertyInfo
		{
			std::string name;
			std::string value;
		};

		std::string obj_name;
		std::list<PropertyInfo> vec_propertys; // 类的所有属性
	};

	std::string class_name; // 该类的名字
	std::list<ClassPropertyDescInfo> vec_desc; // 对该类的一些成员属性的描述
	std::list<ObjectInfo> vec_obj; // 该类下，所有实例化出来的对象名字/对象信息
};

class FileProcess
{
public:
	FileProcess();
	virtual ~FileProcess();

	void setExcelPath(const std::string& path);
	bool load();
	bool save();

	void setUTF8(const bool b);

private:
	bool loadDataFromExcel(const std::string& strFile, const std::string& class_name);
	bool loadDataFromExcel(MiniExcelReader::Sheet& sheet, ClassData* pClassData);

	bool readData(MiniExcelReader::Sheet& sheet, ClassData* pClassData);

	bool saveForObject();
	bool saveForCPP();
	bool saveForXml();

	std::vector<std::string> getFileListInFolder(std::string folderPath, int depth);
	void stringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst);

	void mkdir();

private:

	bool convertIntoUTF8_ = false;

	std::string strRootDir_;
	std::string strCppPath_;
	std::string strXmlPath_;

	ClassData baseObject_;
	std::map<std::string, ClassData*> classData_;
};
