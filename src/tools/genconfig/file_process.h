#pragma once



#include "excel_reader.h"
#include <map>


static const std::string CPP_DIR = "cpp/";
static const std::string STRUCT_DIR_NAME = "struct/";
static const std::string IMPL_DIR_NAME = "impl/";

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


class ClassProperty
{
public:
	ClassProperty()
	{
	}

	std::map<std::string, std::string> descList;//tag, value
	std::string name;
	std::string type;
};

class ClassRecord
{
public:
	ClassRecord()
	{
	}

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

class ClassStruct
{
public:
	ClassStruct()
	{
	}
	std::string strClassName;
	std::map<std::string, ClassProperty*> xPropertyList;//key, desc
	std::map<std::string, ClassRecord*> xRecordList;//name, desc
};

class ClassElement
{
public:
	ClassElement()
	{
	}

	class ElementData
	{
	public:
		std::map<std::string, std::string> xPropertyList;
	};

	std::map<std::string, ElementData*> xElementList;//key, iniList
};

class ClassData
{
public:
	ClassStruct xStructData;
	ClassElement xIniData;
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
	bool loadDataAndProcessProperty(MiniExcelReader::Sheet& sheet, ClassData* pClassData);
	bool loadDataAndProcessComponent(MiniExcelReader::Sheet& sheet, ClassData* pClassData);
	bool loadDataAndProcessRecord(MiniExcelReader::Sheet& sheet, ClassData* pClassData);

	bool saveForCPP();
	bool saveForStruct();
	bool saveForIni();
	bool saveForLogicClass();

	std::vector<std::string> getFileListInFolder(std::string folderPath, int depth);
	void stringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst);

	void mkdir();

private:

	bool convertIntoUTF8_ = false;

	std::string strExcelPath_;
	std::string strXMLStructPath_;
	std::string strXMLImplPath_;
	std::string strCppPath_;

	std::map<std::string, ClassData*> classData_;
};
