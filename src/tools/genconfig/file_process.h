#pragma once



#include "excel_reader.h"
#include <map>


static const std::string CPP_DIR = "cpp/";
static const std::string STRUCT_DIR_NAME = "struct/";
static const std::string IMPL_DIR_NAME = "impl/";		
static const std::string XML_NAME = "xml/";

static const std::string BEGIN_NAMESPACE = "namespace zq\n{\nnamespace config\n{\n";
static const std::string END_NAMESPACE = "\n}}\n";
static const std::string BASE_OBJECT_NAME = "Object";
static const std::string THIS_NAME = "this_name";

static const std::string OBJ_EXCEL_NAME = "Object.xlsx";
static const std::string LOGIC_CLASS_NAME_XML = "LogicClass.xml";

static const std::string CPP_FILE_NAME = "config_define.hpp";

constexpr int PROPERTY_HEIGHT = 10;
constexpr int RECORD_HEIGHT = 13;
constexpr int RECORD_DESC_HEIGHT = 9;


struct ClassProperty
{
	std::map<std::string, std::string> descList;//tag, value
	std::string name;
	std::string type;
};

struct ClassRecord
{
	struct RecordColDesc
	{
		int index;
		std::string type;
		std::string desc;
	};
	
	std::string strClassName;
	std::map<std::string, std::string> descList;//tag, value
	std::map<std::string, RecordColDesc*> colList;//tag, desc
};

struct ClassStruct
{
	std::string strClassName;
	std::map<std::string, ClassProperty*> xPropertyList;//key, desc
	std::map<std::string, ClassRecord*> xRecordList;//name, desc
};

struct ClassElement
{
	struct ElementData
	{
		std::map<std::string, std::string> xPropertyList;
	};

	std::map<std::string, ElementData*> xElementList;//key, iniList
};

struct ClassData
{
	ClassStruct xStructData;
	ClassElement xEleData;

	// 类中的属性信息
	struct PropertyInfo
	{
		// 对该属性的描述，比如是否存档之类的
		struct PropertyDescInfo
		{
			std::string desc_name;
			std::string desc_value;
		};

		std::string name;  // 属性名
		std::string type;  // 属性类型
		std::string value; // 属性值
		std::map<std::string, PropertyDescInfo> propertys_map; // 该属性的所有描述
	};

	struct ObjectInfo
	{
		std::vector<PropertyInfo*> vec_propertys; // 类的所有属性
	};

	std::string obj_name; // 该类的名字
	std::map<std::string, ObjectInfo*> obj_map; // 该类下，所有实例化出来的对象名字/对象信息
};

class FileProcess
{
public:
	FileProcess();
	virtual ~FileProcess();

	void setExcelPath(const std::string& path);
	bool loadDataFromExcel();
	bool save();

	void setUTF8(const bool b);

private:
	bool loadDataFromExcel(const std::string& strFile, const std::string& class_name);
	bool loadDataFromExcel(MiniExcelReader::Sheet& sheet, ClassData* pClassData);

	bool loadIniData(MiniExcelReader::Sheet& sheet, ClassData* pClassData);
	bool readData(MiniExcelReader::Sheet& sheet, ClassData* pClassData);
	bool loadDataAndProcessRecord(MiniExcelReader::Sheet& sheet, ClassData* pClassData);

	bool saveForCPP();
	bool saveForStruct();
	bool saveForIni();
	bool saveForLogicClass();
	bool saveForXml();

	std::vector<std::string> getFileListInFolder(std::string folderPath, int depth);
	void stringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst);

	void mkdir();

private:

	bool convertIntoUTF8_ = false;

	std::string strExcelPath_;
	std::string strXMLStructPath_;
	std::string strXMLImplPath_;
	std::string strCppPath_;
	std::string strXmlPath_;

	std::map<std::string, ClassData*> classData_;
};
