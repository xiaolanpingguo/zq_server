#pragma once


#include "excel_reader.h"
#include <map>
#include <list>
#include <unordered_map>


struct CfgExcelData
{
	struct ObjInfo
	{
		// 类中的属性信息
		struct PropertyInfo
		{
			std::string name;
			std::string type;
			std::string value;
			std::string desc;
		};

		std::string obj_name;
		std::list<PropertyInfo> vec_propertys; // 类的所有属性
	};

	std::string class_name; // 该类的名字
	std::list<ObjInfo> vec_obj; // 该类下，所有实例化出来的对象名字/对象信息
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
	bool loadCsv(const std::string& dir);
	bool readCsv(const std::string& strFile);

private:

	bool loadExcel(const std::string& dir);
	bool readExcel(const std::string& strFile, CfgExcelData* pClassData);
	bool readExcelSheet(MiniExcelReader::Sheet& sheet, CfgExcelData* pClassData);

	bool saveCPP();
	bool saveForXml();

	void getFileListInFolder(std::string folderPath, std::vector<std::string>& result);
	void stringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst);

	void mkdir();

private:

	bool convertIntoUTF8_ = false;

	std::string strRootDir_;
	std::string strCppPath_;
	std::string strXmlPath_;

	std::map<std::string, CfgExcelData*> cfgExcelData_;
};
