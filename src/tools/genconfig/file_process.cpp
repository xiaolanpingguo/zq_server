#include "file_process.h"
#include "utf8_to_gbk.h"
#include <iostream>
#include <sstream>
#include <direct.h>
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
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

#include "dependencies/RapidXML/rapidxml.hpp"
#include "dependencies/RapidXML/rapidxml_iterators.hpp"
#include "dependencies/RapidXML/rapidxml_print.hpp"
#include "dependencies/RapidXML/rapidxml_utils.hpp"	   

#include "csv_parser.hpp"
#include "csv_config.hpp"

// 路径
static const std::string STR_ROOT_DIR = "../datacfg/";
static const std::string STR_EXCEL_DIR = STR_ROOT_DIR + "excel/";
static const std::string STR_CPP_DIR = STR_ROOT_DIR + "cpp/";
static const std::string STR_XML_DIR = STR_ROOT_DIR + "xml/";
static const std::string STR_CSV_DIR = STR_ROOT_DIR + "csv/";

// 写入到cpp的一些常量
static const std::string BEGIN_NAMESPACE = "namespace zq\n{\nnamespace config\n{\n";
static const std::string END_NAMESPACE = "\n}}\n";
static const std::string THIS_NAME = "this_name";

// 生成的cpp头文件名字
static const std::string CPP_FILE_NAME = "excel_cfg_define.hpp";


static constexpr int STATIC_EXCEL_HEIGHT_SEP = 3;


struct CSV_Redis
{
	int id;
	std::string name;
	std::string ip;
	int port;
	std::string auth;
};

inline void zq::TCsvFile<CSV_Redis>::parseRow(zq::CsvParse&& parse)
{
	parse >> thisRow->id
		>> thisRow->name
		>> thisRow->ip
		>> thisRow->port
		>> thisRow->auth;
}


FileProcess::FileProcess()
{

}

FileProcess::~FileProcess()
{

}

void FileProcess::setExcelPath(const std::string& path)
{
	strRootDir_ = path;
	if (strRootDir_[strRootDir_.size() - 1] != '/')
	{
		strRootDir_ += "/";
	}
}

bool FileProcess::load()
{
	if (strRootDir_.empty())
	{
		return false;
	}

	// 创建要生成的文件夹路径
	mkdir();

	bool success = false;
	do 
	{
		//if (!loadExcel(STR_EXCEL_DIR))
		//{
		//	std::cout << "loadDBCfgDir failed." << std::endl;
		//	break;
		//}

		if (!loadCsv(STR_CSV_DIR))
		{
			std::cout << "loadDBCfgDir failed." << std::endl;
			break;
		}

		success = true;
	} while (0);

	if (!success)
	{
		return false;
	}

	return true;
}

bool FileProcess::loadCsv(const std::string& dir)
{
	// 先获得这个路径下的所有文件
	std::vector<std::string> fileList;
	getFileListInFolder(dir, fileList);
	for (auto fileName : fileList)
	{
		stringReplace(fileName, "\\", "/");
		stringReplace(fileName, "//", "/");

		// 后缀名
		auto strExt = fileName.substr(fileName.find_last_of('.') + 1, fileName.length() - fileName.find_last_of('.') - 1);
		if (strExt != "csv")
		{
			continue;
		}

		readCsv(fileName);
		//// 开始读取
		//CfgExcelData* pExcelData = new CfgExcelData();
		//if (!readCsv(fileName, pExcelData))
		//{
		//	std::cout << "read " + fileName + " failed!" << std::endl;
		//	assert(0);
		//	return false;
		//}

		//cfgExcelData_[fileName] = pExcelData;
	}

	return true;
}

bool FileProcess::readCsv(const std::string& strFile)
{
	using namespace zq;
	ConfigSystem::get_instance().create<CSV_Redis>(strFile);

	int id = ConfigSystem::get_instance().getCsvRow<CSV_Redis>(1)->id;
	std::string name = ConfigSystem::get_instance().getCsvRow<CSV_Redis>(1)->name;
	std::string ip = ConfigSystem::get_instance().getCsvRow<CSV_Redis>(1)->ip;
	int port = ConfigSystem::get_instance().getCsvRow<CSV_Redis>(1)->port;
	std::string auth = ConfigSystem::get_instance().getCsvRow<CSV_Redis>(1)->auth;

	std::cout << "id: " << id << std::endl;
	std::cout << "name: " << name << std::endl;
	std::cout << "ip: " << ip << std::endl;
	std::cout << "port: " << port << std::endl;
	std::cout << "auth: " << auth << std::endl;
	return true;
}

bool FileProcess::loadExcel(const std::string& dir)
{
	// 先获得这个路径下的所有文件
	std::vector<std::string> fileList;
	getFileListInFolder(dir, fileList);
	for (auto fileName : fileList)
	{
		stringReplace(fileName, "\\", "/");
		stringReplace(fileName, "//", "/");

		if (fileName.find("$") != std::string::npos)
		{
			continue;
		}

		// 后缀名
		auto strExt = fileName.substr(fileName.find_last_of('.') + 1, fileName.length() - fileName.find_last_of('.') - 1);
		if (strExt != "xlsx")
		{
			continue;
		}

		// 开始读取
		CfgExcelData* pExcelData = new CfgExcelData();
		if (!readExcel(fileName, pExcelData))
		{
			std::cout << "read " + fileName + " failed!" << std::endl;
			assert(0);
			return false;
		}

		cfgExcelData_[fileName] = pExcelData;
	}

	return true;
}

bool FileProcess::readExcel(const std::string& strFile, CfgExcelData* pClassData)
{
	// 去掉了后缀的文件名
	auto class_name = strFile.substr(strFile.find_last_of('/') + 1, strFile.find_last_of('.') - strFile.find_last_of('/') - 1);
	pClassData->class_name = class_name;

	// 打开文件
	MiniExcelReader::ExcelFile* xExcel = new MiniExcelReader::ExcelFile();
	if (!xExcel->open(strFile.c_str()))
	{
		std::cout << "can't open" << strFile << std::endl;
		return false;
	}

	std::cout << "begin read file: " << strFile << std::endl;

	// 获得所有的sheet(表格)，就是文件下面一栏的标签栏
	std::vector<MiniExcelReader::Sheet>& sheets = xExcel->sheets();
	for (size_t i =0; i < 1; ++i)
	{
		MiniExcelReader::Sheet& sh = sheets[i];

		std::cout << "---beginread sheet: " << sh.getName() << std::endl;

		// 转换一下，全部变为小写
		std::string strSheetName = sh.getName();
		transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

		// 开始读取sheet的内容
		if (!readExcelSheet(sh, pClassData))
		{
			return false;
		}
	}

	return true;
}

bool FileProcess::readExcelSheet(MiniExcelReader::Sheet& sheet, CfgExcelData* pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();

	// 每一行，从第STATIC_EXCEL_HEIGHT_SEP + 1开始
	for (int row = STATIC_EXCEL_HEIGHT_SEP + 1; row <= dim.lastRow; ++row)
	{
		// 实例化出来的对象名字，在第一列
		CfgExcelData::ObjInfo obj;
		obj.obj_name = sheet.getCell(row, 1)->value;

		// 读取属性名，从第二列开始读取
		for (int col = dim.firstCol + 1; col <= dim.lastCol; ++col)
		{
			std::string prop_name = sheet.getCell(1, col)->value;
			std::string prop_type = sheet.getCell(2, col)->value;
			std::string prop_desc = sheet.getCell(3, col)->value;
			std::string prop_value = sheet.getCell(row, col)->value;

			CfgExcelData::ObjInfo::PropertyInfo property_info;
			property_info.name = prop_name;
			property_info.type = prop_type;
			property_info.value = prop_value;
			property_info.desc = prop_desc;

			obj.vec_propertys.emplace_back(property_info);
		}

		// 添加一个对象
		pClassData->vec_obj.emplace_back(obj);
	}

	return true;
}

bool FileProcess::save()
{
	saveForXml();
	saveCPP();
	return true;
}

bool FileProcess::saveCPP()
{
	// 所有的类
	for (const auto& class_data : cfgExcelData_)
	{
		std::string class_name = class_data.second->class_name;
		std::string filename = strCppPath_ + class_name + ".hpp";
		std::ofstream out(filename);
		if (!out.is_open())
		{
			assert(false);
			return false;
		}

		std::string strFileHead = "#pragma once\n\n" + std::string("#include <string>\n\n") + BEGIN_NAMESPACE;
		out << strFileHead;

		out << "\tstruct " + class_name + "\n\t{\n";
		out << "\t\tstatic const std::string& " + THIS_NAME + "(){ static std::string x = \"" + class_name + "\"; return x; };\n";
		for (const auto& obj : class_data.second->vec_obj)
		{
			for (const auto& prop : obj.vec_propertys)
			{
				std::string property_name = prop.name;
				out << "\t\tstatic const std::string& " + property_name + "(){ static std::string x = \"" + property_name + "\"; return x; };\n";
			}

			out << "\t};\n";
		}

		out << END_NAMESPACE;
		out.close();
	}

	return true;
}

bool FileProcess::saveForXml()
{
	// 所有的类
	for (const auto& class_data : cfgExcelData_)
	{
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
		doc.append_node(root);

		// 开始写入每个对象的节点
		std::string class_name = class_data.second->class_name;
		rapidxml::xml_node<>* class_node = doc.allocate_node(rapidxml::node_element, class_name.c_str(), NULL);
		for (const auto& obj : class_data.second->vec_obj)
		{
			// 对象节点
			const std::string& obj_name = obj.obj_name;
			rapidxml::xml_node<>* obj_node = doc.allocate_node(rapidxml::node_element, obj_name.c_str(), NULL);

			// 一个对象下的所有属性
			for (const auto& prop : obj.vec_propertys)
			{
				obj_node->append_attribute(doc.allocate_attribute(prop.name.c_str(), prop.value.c_str()));
			}

			// 加入到这个类
			class_node->append_node(obj_node);
		}

		// 这个类加入到这个文档
		doc.append_node(class_node);

		// 写入文件
		std::string wfile = STR_XML_DIR + class_name + ".xml";
		std::ofstream out(wfile);
		if (!out.is_open())
		{
			std::cout << "!!!!!!open file faild, file: " << wfile << std::endl;
			continue;
		}

		out << doc;
		out.close();
	}

	return true;
}

void FileProcess::setUTF8(const bool b)
{
	convertIntoUTF8_ = b;
}

void FileProcess::getFileListInFolder(std::string folderPath, std::vector<std::string>& result)
{
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
}

void FileProcess::stringReplace(std::string & strBig, const std::string & strsrc, const std::string & strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

void FileProcess::mkdir()
{
	_mkdir(STR_EXCEL_DIR.c_str());
	_mkdir(STR_CPP_DIR.c_str());
	_mkdir(STR_XML_DIR.c_str());
}
