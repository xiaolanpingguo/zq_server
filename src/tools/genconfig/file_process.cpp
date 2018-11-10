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


FileProcess::FileProcess()
{
	strExcelPath_ = "../datacfg/excel/";
	strXMLStructPath_ = strExcelPath_ + STRUCT_DIR_NAME;
	strXMLImplPath_ = strExcelPath_ + IMPL_DIR_NAME;
	strCppPath_ = strExcelPath_ + CPP_DIR;
	strXmlPath_ = strExcelPath_ + XML_NAME;
}

FileProcess::~FileProcess()
{

}

void FileProcess::setExcelPath(const std::string& path)
{
	strExcelPath_ = path;
	if (strExcelPath_[strExcelPath_.size() - 1] != '/')
	{
		strExcelPath_ += "/";
	}

	strXMLStructPath_ = strExcelPath_ + STRUCT_DIR_NAME;
	strXMLImplPath_ = strExcelPath_ + IMPL_DIR_NAME;
	strCppPath_ = strExcelPath_ + CPP_DIR;

	std::cout << "strXMLStructPath_:" << strXMLStructPath_ << std::endl;
	std::cout << "strXMLImplPath_:" << strXMLImplPath_ << std::endl;
	std::cout << "strCppPath_:" << strCppPath_ << std::endl;
}

bool FileProcess::loadDataFromExcel()
{
	if (strExcelPath_.empty())
	{
		return false;
	}

	// 创建要生成的文件夹路径
	mkdir();

	// 先加载Object.xlsx文件
	std::string objfilename = strExcelPath_ + OBJ_EXCEL_NAME;
	loadDataFromExcel(objfilename.c_str(), BASE_OBJECT_NAME);

	// 先获得这个路径下的所有文件
	auto fileList = getFileListInFolder(strExcelPath_, 0);
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
		if (strFileName == BASE_OBJECT_NAME)
		{
			continue;
		}

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

	// 这里把去除了后缀的文件名就抽象成类名
	ClassData* pClassData = new ClassData();
	pClassData->obj_name = class_name;

	classData_[class_name] = pClassData;
	pClassData->xStructData.strClassName = class_name;

	// 打开文件
	MiniExcelReader::ExcelFile* xExcel = new MiniExcelReader::ExcelFile();
	if (!xExcel->open(strFile.c_str()))
	{
		std::cout << "can't open" << strFile << std::endl;
		return false;
	}

	// 获得所有的sheet(表格)，就是文件下面一栏的标签栏
	std::vector<MiniExcelReader::Sheet>& sheets = xExcel->sheets();
	for (MiniExcelReader::Sheet& sh : sheets)
	{
		std::cout << "begin load file: " << strFile << std::endl;
		loadDataFromExcel(sh, pClassData);
	}

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
		//loadIniData(sheet, pClassData);
	}
	else if (strSheetName.find("record") != std::string::npos)
	{
		loadDataAndProcessRecord(sheet, pClassData);
	}
	else
	{
		std::cout << pClassData->xStructData.strClassName << " " << strSheetName << std::endl;
		assert(0);
	}

	return true;
}

bool FileProcess::loadIniData(MiniExcelReader::Sheet & sheet, ClassData * pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();
	std::string strSheetName = sheet.getName();
	transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

	std::map<std::string, int> PropertyIndex;//col index
	for (int c = dim.firstCol + 1; c <= dim.lastCol; c++)
	{
		MiniExcelReader::Cell* cell = sheet.getCell(dim.firstRow, c);
		PropertyIndex[cell->value] = c;
	}

	for (int r = dim.firstRow + PROPERTY_HEIGHT; r <= dim.lastRow; r++)
	{
		MiniExcelReader::Cell* pIDCell = sheet.getCell(r, dim.firstCol);
		if (pIDCell && !pIDCell->value.empty())
		{
			ClassElement::ElementData* pIniObject = new ClassElement::ElementData();
			pClassData->xEleData.xElementList[pIDCell->value] = pIniObject;

			for (auto itProperty = PropertyIndex.begin(); itProperty != PropertyIndex.end(); ++itProperty)
			{
				std::string strPropertyName = itProperty->first;
				int nCol = itProperty->second;

				MiniExcelReader::Cell* cell = sheet.getCell(r, nCol);
				if (cell)
				{
					pIniObject->xPropertyList[strPropertyName] = cell->value;
				}
				else
				{
					pIniObject->xPropertyList[strPropertyName] = "";
				}
			}
		}
	}
	
	return false;
}

bool FileProcess::readData(MiniExcelReader::Sheet& sheet, ClassData* pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();

	// excel的读取是从第一行第一列开始，符合人类的习惯，因为不是从0开始
	// 行，不读取第一行，接下来的读取都不用判断指针，好提前发现配置错误
	auto obj = new ClassData::ObjectInfo();
	auto property_info = new ClassData::PropertyInfo();

	// 读取属性名，从第二列开始读取
	for (int col = dim.firstCol + 1; col <= dim.lastCol; ++col)
	{
		ClassData::PropertyInfo::PropertyDescInfo property_desc_info;

		// 第一行是该属性的名称
		MiniExcelReader::Cell* cell_1 = sheet.getCell(1, col);
		std::string property_name = cell_1->value;

		// 开始读取对该属性的描述，从第二行开始读取
		for (int row = dim.firstRow + 1; row <= dim.lastRow; ++row)
		{
			MiniExcelReader::Cell* cell_2 = sheet.getCell(row, col);

			// 这里把它写死了的，第二行到PROPERTY_HEIGHT，全是对该属性的描述
			if (row <= PROPERTY_HEIGHT)
			{
				// 描述的名称，配置是在第一列，比如type, save之类的
				std::string desc_name = sheet.getCell(row, 1)->value;

				// 具体的值，一般是0或者1
				std::string desc_value = cell_2->value;
				property_desc_info.desc_name = desc_name;
				property_desc_info.desc_value = desc_value;

				property_info->propertys_map[desc_name] = property_desc_info;
			}
			// 从PROPERTY_HEIGHT到最后一行，是具体的每个对象的配置
			else
			{
				// 这里专门取下该属性的类型
				{
					auto it = property_info->propertys_map.find("type");
					property_info->type = it->second.desc_value;
				}

				// 具体的属性的名称和对应的值
				property_info->name = property_name;
				property_info->value = cell_2->value;

				// 添加一个属性
				obj->vec_propertys.push_back(property_info);

				// 添加一个对象
				pClassData->obj_map[pClassData->obj_name] = obj;
			}
		}	
	}

	return false;
}

bool FileProcess::loadDataAndProcessRecord(MiniExcelReader::Sheet & sheet, ClassData * pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();
	std::string strSheetName = sheet.getName();
	transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

	for (int nIndex = 0; nIndex < 100; nIndex++)
	{
		int nStartRow = nIndex * RECORD_HEIGHT + 1;
		int nEndRow = (nIndex + 1) * RECORD_HEIGHT;

		MiniExcelReader::Cell* pTestCell = sheet.getCell(nStartRow, dim.firstCol);
		if (pTestCell)
		{
			MiniExcelReader::Cell* pNameCell = sheet.getCell(nStartRow, dim.firstCol + 1);
			std::string strRecordName = pNameCell->value;
			
			////////////

			ClassRecord* pClassRecord = new ClassRecord();
			pClassData->xStructData.xRecordList[strRecordName] = pClassRecord;
			////////////

			for (int r = nStartRow + 1; r <= nStartRow + RECORD_DESC_HEIGHT; r++)
			{
				MiniExcelReader::Cell* cellDesc = sheet.getCell(r, dim.firstCol);
				MiniExcelReader::Cell* cellValue = sheet.getCell(r, dim.firstCol + 1);

				pClassRecord->descList[cellDesc->value] = cellValue->value;
			}

			int nRecordCol = atoi(pClassRecord->descList["Col"].c_str());
			for (int c = dim.firstCol; c <= nRecordCol; c++)
			{
				int r = nStartRow + RECORD_DESC_HEIGHT + 1;
				MiniExcelReader::Cell* pCellColName = sheet.getCell(r, c);
				MiniExcelReader::Cell* pCellColType = sheet.getCell(r + 1, c);
				MiniExcelReader::Cell* pCellColDesc = sheet.getCell(r + 2, c);

				ClassRecord::RecordColDesc* pRecordColDesc = new ClassRecord::RecordColDesc();
				pRecordColDesc->index = c - 1;
				pRecordColDesc->type = pCellColType->value;
				if (pCellColDesc)
				{
					pRecordColDesc->desc = pCellColDesc->value;
				}

				pClassRecord->colList[pCellColName->value] = pRecordColDesc;
			}
		}
	}

	return true;
}

bool FileProcess::save()
{
	saveForCPP();
	saveForStruct();
	saveForIni();
	saveForLogicClass();
	saveForXml();

	return false;
}

bool FileProcess::saveForCPP()
{
	// 在excel的路径下生成cpp的文件夹
	std::string filename = strCppPath_ + CPP_FILE_NAME;
	FILE* hppWriter = fopen(filename.c_str(), "w");

	std::string strFileHead;

	strFileHead = strFileHead
	+ "#pragma once\n\n"
	+ "#include <string>\n"
	+ BEGIN_NAMESPACE;

	fwrite(strFileHead.c_str(), strFileHead.length(), 1, hppWriter);

	ClassData* pBaseObject = classData_[BASE_OBJECT_NAME];
	std::string instanceField = "\n";

	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		const std::string& strClassName = it->first;
		ClassData* pClassDta = it->second;
		// cpp
		std::string strPropertyInfo;
		
		strPropertyInfo += "\tclass " + strClassName + "\n\t{\n\tpublic:\n";
		strPropertyInfo += "\t\t//Class name\n\t";
		strPropertyInfo += "\tstatic const std::string& " + THIS_NAME + "(){ static std::string x = \"" + strClassName + "\"; return x; };";

		instanceField += "\tconst std::string " + strClassName + "::" + THIS_NAME + " = \"" + strClassName + "\";\n";

		if (strClassName != BASE_OBJECT_NAME)
		{
			//add base class properties
			strPropertyInfo += "\t\t// Object\n";
			
			for (auto itProperty = pBaseObject->xStructData.xPropertyList.begin(); itProperty != pBaseObject->xStructData.xPropertyList.end(); ++itProperty)
			{
				const std::string& strPropertyName = itProperty->first;
				ClassProperty* pClassProperty = itProperty->second;

				strPropertyInfo += "\t\tstatic const std::string& " + strPropertyName + "(){ static std::string x = \"" + strPropertyName + "\"; return x; };";
				strPropertyInfo += "// " + pClassProperty->descList["Type"] + "\n";

				instanceField += "\tconst std::string " + strClassName + "::" + strPropertyName + " = \"" + strPropertyName + "\";\n";
			}
		}

		strPropertyInfo += "\t\t// Property\n";
		for (auto itProperty = pClassDta->xStructData.xPropertyList.begin(); itProperty != pClassDta->xStructData.xPropertyList.end(); ++itProperty)
		{
			const std::string& strPropertyName = itProperty->first;
			ClassProperty* pClassProperty = itProperty->second;

			strPropertyInfo += "\t\tstatic const std::string& " + strPropertyName + "(){ static std::string x = \"" + strPropertyName + "\"; return x; };";
			strPropertyInfo += "// " + pClassProperty->descList["Type"] + "\n";

			instanceField += "\tconst std::string " + strClassName + "::" + strPropertyName + " = \"" + strPropertyName + "\";\n";
		}

		fwrite(strPropertyInfo.c_str(), strPropertyInfo.length(), 1, hppWriter);

		//record
		std::string strRecordInfo = "";
		strRecordInfo += "\t\t// Record\n";

		for (auto itRecord = pClassDta->xStructData.xRecordList.begin(); itRecord != pClassDta->xStructData.xRecordList.end(); ++itRecord)
		{
			const std::string& strRecordName = itRecord->first;
			ClassRecord* pClassRecord = itRecord->second;

			std::cout << "save for cpp ---> " << strClassName  << "::" << strRecordName << std::endl;

			strRecordInfo += "\t\tclass " + strRecordName + "\n\t\t{\n\t\tpublic:\n";
			strRecordInfo += "\t\t\t//Class name\n\t";
			strRecordInfo += "\t\tstatic const std::string& " + THIS_NAME + "(){ static std::string x = \"" + strRecordName + "\"; return x; };\n";

			instanceField += "\tconst std::string " + strClassName + "::" + strRecordName + "::" + THIS_NAME + " = \"" + strRecordName + "\";\n";

			//col
			for (int i = 0; i < pClassRecord->colList.size(); ++i)
			{
				for (std::map<std::string, ClassRecord::RecordColDesc*>::iterator itCol = pClassRecord->colList.begin();
					itCol != pClassRecord->colList.end(); ++itCol)
				{
					const std::string& colTag = itCol->first;
					ClassRecord::RecordColDesc* pRecordColDesc = itCol->second;

					if (pRecordColDesc->index == i)
					{
						strRecordInfo += "\t\t\tstatic const int " + colTag + " = " + std::to_string(pRecordColDesc->index) + ";//" + pRecordColDesc->type + "\n";
					}
				}
			}

			strRecordInfo += "\n\t\t};\n";
		}

		fwrite(strRecordInfo.c_str(), strRecordInfo.length(), 1, hppWriter);

		std::string strClassEnd;
		strClassEnd += "\n\t};\n";
		fwrite(strClassEnd.c_str(), strClassEnd.length(), 1, hppWriter);
	}

	//fwrite(instanceField.c_str(), instanceField.length(), 1, hppWriter);

	std::string strFileEnd = END_NAMESPACE;
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, hppWriter);

	fclose(hppWriter);

	return false;
}

bool FileProcess::saveForStruct()
{
	ClassData* pBaseObject = classData_[BASE_OBJECT_NAME];
	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		const std::string& strClassName = it->first;
		ClassData* pClassDta = it->second;

		std::cout << "save for struct ---> " << strClassName << std::endl;

		std::string strFileName = strXMLStructPath_ + strClassName + ".xml";
		FILE* structWriter = fopen(strFileName.c_str(), "w");

		std::string strFileHead = "<?xml version='1.0' encoding='utf-8' ?>\n<XML>\n";
		fwrite(strFileHead.c_str(), strFileHead.length(), 1, structWriter);

		std::string strFilePrpertyBegin = "\t<Propertys>\n";
		fwrite(strFilePrpertyBegin.c_str(), strFilePrpertyBegin.length(), 1, structWriter);

		for (auto itProperty = pClassDta->xStructData.xPropertyList.begin(); itProperty != pClassDta->xStructData.xPropertyList.end(); ++itProperty)
		{
			const std::string& strPropertyName = itProperty->first;
			ClassProperty* xPropertyData = itProperty->second;

			std::string strElementData = "\t\t<Property Id=\"" + strPropertyName + "\" ";
			for (auto itDesc = xPropertyData->descList.begin(); itDesc != xPropertyData->descList.end(); ++itDesc)
			{
				const std::string& strKey = itDesc->first;
				const std::string& strValue = itDesc->second;
				strElementData += strKey + "=\"" + strValue + "\" ";
			}
			strElementData += "/>\n";
			fwrite(strElementData.c_str(), strElementData.length(), 1, structWriter);
		}

		std::string strFilePropertyEnd = "\t</Propertys>\n";
		fwrite(strFilePropertyEnd.c_str(), strFilePropertyEnd.length(), 1, structWriter);

		std::string strFileRecordBegin = "\t<Records>\n";
		fwrite(strFileRecordBegin.c_str(), strFileRecordBegin.length(), 1, structWriter);

		for (auto itRecord = pClassDta->xStructData.xRecordList.begin(); itRecord != pClassDta->xStructData.xRecordList.end(); ++itRecord)
		{
			const std::string& strRecordName = itRecord->first;
			ClassRecord* xRecordData = itRecord->second;

			//for desc
			std::string strElementData = "\t\t<Record Id=\"" + strRecordName + "\" ";
			for (std::map<std::string, std::string>::iterator itDesc = xRecordData->descList.begin();
				itDesc != xRecordData->descList.end(); ++itDesc)
			{
				const std::string& strKey = itDesc->first;
				const std::string& strValue = itDesc->second;
				strElementData += strKey + "=\"" + strValue + "\"\t ";
			}
			strElementData += ">\n";

			//for col list
			for (int i = 0; i < xRecordData->colList.size(); ++i)
			{
				for (auto itDesc = xRecordData->colList.begin(); itDesc != xRecordData->colList.end(); ++itDesc)
				{
					const std::string& strKey = itDesc->first;
					const ClassRecord::RecordColDesc* pRecordColDesc = itDesc->second;

					if (pRecordColDesc->index == i)
					{
						strElementData += "\t\t\t<Col Type =\"" + pRecordColDesc->type + "\"\tTag=\"" + strKey + "\"/>";
						if (!pRecordColDesc->desc.empty())
						{
							strElementData += "<!--- " + pRecordColDesc->desc + "-->\n";
						}
						else
						{
							strElementData += "\n";
						}
					}
				}
			}
			
			strElementData += "\t\t</Record>\n";
			fwrite(strElementData.c_str(), strElementData.length(), 1, structWriter);
		}

		std::string strFilePrpertyEnd = "\t</Records>\n";
		fwrite(strFilePrpertyEnd.c_str(), strFilePrpertyEnd.length(), 1, structWriter);

		std::string strFileEnd = "</XML>";
		fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, structWriter);

		fclose(structWriter);
	}

	return false;
}

bool FileProcess::saveForIni()
{
	ClassData* pBaseObject = classData_[BASE_OBJECT_NAME];
	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		const std::string& strClassName = it->first;
		ClassData* pClassDta = it->second;

		std::cout << "save for ini ---> " << strClassName << std::endl;

		std::string strFileName = strXMLImplPath_ + strClassName + ".xml";
		FILE* iniWriter = fopen(strFileName.c_str(), "w");

		std::string strFileHead = "<?xml version='1.0' encoding='utf-8' ?>\n<XML>\n";
		fwrite(strFileHead.c_str(), strFileHead.length(), 1, iniWriter);

		for (auto itElement = pClassDta->xEleData.xElementList.begin(); itElement != pClassDta->xEleData.xElementList.end(); ++itElement)
		{
			const std::string& strElementName = itElement->first;
			ClassElement::ElementData* pIniData = itElement->second;

			std::string strElementData = "\t<Object Id=\"" + strElementName + "\" ";
			for (std::map<std::string, std::string>::iterator itProperty = pIniData->xPropertyList.begin();
				itProperty != pIniData->xPropertyList.end(); ++itProperty)
			{
				const std::string& strKey = itProperty->first;
				const std::string& strValue = itProperty->second;
				strElementData += strKey + "=\"" + strValue + "\" ";
			}
			strElementData += "/>\n";
			fwrite(strElementData.c_str(), strElementData.length(), 1, iniWriter);
		}

		std::string strFileEnd = "</XML>";
		fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, iniWriter);

		fclose(iniWriter);
	}

	return false;
}

bool FileProcess::saveForLogicClass()
{
	std::string strFileName = strXMLStructPath_ + LOGIC_CLASS_NAME_XML;

	FILE* iniWriter = fopen(strFileName.c_str(), "w");

	std::string strFileHead = "<?xml version='1.0' encoding='utf-8' ?>\n<XML>\n";
	fwrite(strFileHead.c_str(), strFileHead.length(), 1, iniWriter);

	ClassData* pBaseObject = classData_[BASE_OBJECT_NAME];

	std::string strElementData;
	strElementData += "\t<Class Id=\"" + pBaseObject->xStructData.strClassName + "\"\t";
	strElementData += "Path=\"" + STRUCT_DIR_NAME + pBaseObject->xStructData.strClassName + ".xml\"\t";
	strElementData += "InstancePath=\"" + IMPL_DIR_NAME + pBaseObject->xStructData.strClassName + ".xml\"\t>\n";

	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		const std::string& strClassName = it->first;
		ClassData* pClassDta = it->second;
		if (strClassName == "Object")
		{
			continue;
		}

		strElementData += "\t\t<Class Id=\"" + pClassDta->xStructData.strClassName + "\"\t";
		strElementData += "Path=\"" + STRUCT_DIR_NAME + pClassDta->xStructData.strClassName + ".xml\"\t";
		strElementData += "InstancePath=\"" + IMPL_DIR_NAME + pClassDta->xStructData.strClassName + ".xml\"\t/>\n";

	}

	strElementData += "\t</Class>\n";
	fwrite(strElementData.c_str(), strElementData.length(), 1, iniWriter);

	std::string strFileEnd = "</XML>";
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, iniWriter);

	fclose(iniWriter);

	return false;
}

bool FileProcess::saveForXml()
{
	for (auto it = classData_.begin(); it != classData_.end(); ++it)
	{
		const std::string& strClassName = it->first;
		ClassData* pClassDta = it->second;

		std::cout << "save for xml ---> " << strClassName << std::endl;

		// 文件名，以类名命名
		std::string strFileName = strXmlPath_ + strClassName + ".xml";
		std::string strFileHead = "<?xml version='1.0' encoding='utf-8' ?>\n<XML>\n";

		// xml头信息
		FILE* fw = fopen(strFileName.c_str(), "w");
		fwrite(strFileHead.c_str(), strFileHead.length(), 1, fw);

		// 所有的对象
		for (const auto& obj : pClassDta->obj_map)
		{
			const std::string& obj_name = obj.first;
			ClassData::ObjectInfo* obj_info = obj.second;

			std::ostringstream os;
			os << "\t<" << obj_name << "\">\n\t";

			// 该对象下的所有属性
			for (const auto& property_info : obj_info->vec_propertys)
			{
				const std::string& name = property_info->name;
				const std::string& type = property_info->type;
				const std::string& value = property_info->value;

				os << name + "=\"" + name + "\" ";
				os << type + "=\"" + type + "\" ";
				os << value + "=\"" + value + "\" ";
			}

			os << "</" << obj_name << ">\n\t";
			fwrite(os.str().c_str(), os.str().length(), 1, fw);
		}

		// 关闭
		std::string strFileEnd = "</XML>";
		fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, fw);
		fclose(fw);
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
	_mkdir(strXMLStructPath_.c_str());
	_mkdir(strXMLImplPath_.c_str());
	_mkdir(strCppPath_.c_str());
	_mkdir(strXmlPath_.c_str());
}
