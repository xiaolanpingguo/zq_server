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


FileProcess::FileProcess()
{
	strRootDir_ = "../datacfg/";
	strCppPath_ = strRootDir_ + CPP_DIR;
	strXmlPath_ = strRootDir_ + XML_NAME;
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

	std::cout << "xml_path:" << strXmlPath_ << std::endl;
}

bool FileProcess::load()
{
	if (strRootDir_.empty())
	{
		return false;
	}

	// 创建要生成的文件夹路径
	mkdir();

	// 先获得这个路径下的所有文件
	auto fileList = getFileListInFolder(strRootDir_, 0);
	for (auto fileName : fileList)
	{
		stringReplace(fileName, "\\", "/");
		stringReplace(fileName, "//", "/");

		if ((int)(fileName.find("$")) != -1)
		{
			continue;
		}

		// 后缀名
		auto strExt = fileName.substr(fileName.find_last_of('.') + 1, fileName.length() - fileName.find_last_of('.') - 1);
		if (strExt != "xlsx")
		{
			continue;
		}

		// 去掉了后缀的文件名
		auto strFileName = fileName.substr(fileName.find_last_of('/') + 1, fileName.find_last_of('.') - fileName.find_last_of('/') - 1);

		// 开始读取
		if (!loadDataFromExcel(fileName, strFileName))
		{
			std::cout << "Create " + fileName + " failed!" << std::endl;
			return false;
		}
	}

	return true;
}

bool FileProcess::loadDataFromExcel(const std::string& strFile, const std::string& class_name)
{
	if (classData_.find(class_name) != classData_.end())
	{
		std::cout << strFile << " exist!!!" << std::endl;
		return false;
	}

	// 打开文件
	MiniExcelReader::ExcelFile* xExcel = new MiniExcelReader::ExcelFile();
	if (!xExcel->open(strFile.c_str()))
	{
		std::cout << "can't open" << strFile << std::endl;
		return false;
	}

	// 这里把去除了后缀的文件名就抽象成类名
	ClassData* pClassData = new ClassData();
	pClassData->class_name = class_name;

	// 获得所有的sheet(表格)，就是文件下面一栏的标签栏
	std::vector<MiniExcelReader::Sheet>& sheets = xExcel->sheets();
	for (MiniExcelReader::Sheet& sh : sheets)
	{
		std::cout << "begin load file: " << strFile << std::endl;
		loadDataFromExcel(sh, pClassData);
	}

	classData_[class_name] = pClassData;

	return true;
}

bool FileProcess::loadDataFromExcel(MiniExcelReader::Sheet& sheet, ClassData* pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();

	// 转换一下，全部变为小写
	std::string strSheetName = sheet.getName();
	transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

	// 开始读取sheet的内容
	if (strSheetName.find("property") != std::string::npos)
	{
		readData(sheet, pClassData);
	}
	else
	{
		std::cout << pClassData->class_name << " " << strSheetName << std::endl;
		assert(0);
	}

	return true;
}

bool FileProcess::readData(MiniExcelReader::Sheet& sheet, ClassData* pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();

	int separator_row = -1;
	for (int row = dim.firstRow; row <= dim.lastRow; ++row)
	{
		std::string name = sheet.getCell(row, 1)->value;
		if (name == STR_SEPARATOR)
		{
			separator_row = row;
			break;
		}
	}
	if (separator_row == -1)
	{
		assert(false);
		return false;
	}

	// 先读取这个类的所有的成员属性的描述, 名字是第一列，值是第二列
	for (int col = dim.firstCol + 1; col <= dim.lastCol; ++col)
	{
		ClassData::ClassPropertyDescInfo property_info;
		property_info.property_name = sheet.getCell(1, col)->value;

		for (int row = dim.firstRow + 1; row <= separator_row; ++row)
		{
			ClassData::ClassPropertyDescInfo::DescInfo property_desc_info;
			property_desc_info.desc_name = sheet.getCell(row, 1)->value;

			// 不写分隔符
			if (property_desc_info.desc_name == STR_SEPARATOR)
			{
				continue;
			}

			property_desc_info.desc_value = sheet.getCell(row, col)->value;
			property_info.vec_desc_info.emplace_back(property_desc_info);
		}

		pClassData->vec_desc.emplace_back(property_info);
	}

	// 再来读取每个对象
	for (int row = separator_row + 1; row <= dim.lastRow; ++row)
	{
		// 实例化出来的对象名字，在第一列
		ClassData::ObjectInfo obj;
		std::string obj_name = sheet.getCell(row, 1)->value;
		obj.obj_name = obj_name;

		// 读取属性名，从第二列开始读取
		for (int col = dim.firstCol + 1; col <= dim.lastCol; ++col)
		{
			ClassData::ObjectInfo::PropertyInfo property_info;
			property_info.name = sheet.getCell(1, col)->value;	
			auto aaa = sheet.getCell(row, col);
			property_info.value = sheet.getCell(row, col)->value;
			obj.vec_propertys.emplace_back(property_info);
		}

		// 添加一个对象
		pClassData->vec_obj.emplace_back(obj);
	}

	return true;
}

bool FileProcess::save()
{
	saveForObject();
	saveForCPP();
	saveForXml();

	return true;
}

bool FileProcess::saveForObject()
{
	return true;
}

bool FileProcess::saveForCPP()
{
	std::string filename = strCppPath_ + CPP_FILE_NAME;
	std::ofstream out(filename);
	if (!out.is_open())
	{
		assert(false);
		return false;
	}

	std::string strFileHead = "#pragma once\n\n" + std::string("#include <string>\n\n") + BEGIN_NAMESPACE;
	out << strFileHead;

	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		ClassData* class_data = it->second;
		std::string class_name = class_data->class_name;

		out << "\tstruct " + class_name + "\n\t{\n";
		out << "\t\tstatic const std::string& " + THIS_NAME + "(){ static std::string x = \"" + class_name + "\"; return x; };\n";
		
		for (const auto& property : class_data->vec_desc)
		{
			std::string property_name = property.property_name;
			out << "\t\tstatic const std::string& " + property_name + "(){ static std::string x = \"" + property_name + "\"; return x; };\n";
		}

		out << "\t};\n";
	}

	out << END_NAMESPACE;

	out.close();

	return false;
}

bool FileProcess::saveForXml()
{
	// 所有的类
	for (const auto& class_data : classData_)
	{
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
		doc.append_node(root);

		rapidxml::xml_node<>* property_desc_node = doc.allocate_node(rapidxml::node_element, "property_desc", NULL);

		// 写入该类的所有成员的描述
		for (const auto& desc : class_data.second->vec_desc)
		{
			const std::string& property_name = desc.property_name;
			rapidxml::xml_node<>* property_node = doc.allocate_node(rapidxml::node_element, property_name.c_str(), NULL);

			for (const auto& desc_info : desc.vec_desc_info)
			{
				const std::string& desc_name = desc_info.desc_name;
				const std::string& desc_value = desc_info.desc_value;
				property_node->append_attribute(doc.allocate_attribute(desc_name.c_str(), desc_value.c_str()));
			}

			property_desc_node->append_node(property_node);
		}

		doc.append_node(property_desc_node);

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
				const std::string& property_name = prop.name;
				const std::string& property_valye = prop.value;
				obj_node->append_attribute(doc.allocate_attribute(property_name.c_str(), property_valye.c_str()));
			}

			// 加入到这个类
			class_node->append_node(obj_node);
		}

		// 这个类加入到这个文档
		doc.append_node(class_node);

		// 写入文件
		std::ofstream out(strXmlPath_ + class_name + ".xml");
		if (!out.is_open())
		{
			std::cout << "open file faild, file: " << strXmlPath_ << std::endl;
			continue;
		}

		out << doc;
	}

	return true;
}

void FileProcess::setUTF8(const bool b)
{
	convertIntoUTF8_ = b;
}

std::vector<std::string> FileProcess::getFileListInFolder(std::string folderPath, int depth)
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
	_mkdir(strCppPath_.c_str());
	_mkdir(strXmlPath_.c_str());
}
