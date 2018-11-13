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

// ���ñ�����������Ͷ�����зָ��ʶ
static const std::string STR_SEPARATOR = "separator##";

struct ClassData
{
	// �Ը����Ե����������֣����ͣ�ֵ
	struct ClassPropertyDescInfo
	{
		std::string property_name;

		struct DescInfo
		{
			std::string desc_name;
			std::string desc_value;
		};

		std::list<DescInfo> vec_desc_info; // �Ը����Ե���������
	};

	struct ObjectInfo
	{
		// ���е�������Ϣ
		struct PropertyInfo
		{
			std::string name;
			std::string value;
		};

		std::string obj_name;
		std::list<PropertyInfo> vec_propertys; // �����������
	};

	std::string class_name; // ���������
	std::list<ClassPropertyDescInfo> vec_desc; // �Ը����һЩ��Ա���Ե�����
	std::list<ObjectInfo> vec_obj; // �����£�����ʵ���������Ķ�������/������Ϣ
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
