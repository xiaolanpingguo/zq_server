#include "file_process.h"
#include "utf8_to_gbk.h"
#include <iostream>
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
	classData_[class_name] = pClassData;
	pClassData->xStructData.strClassName = class_name;

	// 打开文件
	MiniExcelReader::ExcelFile* xExcel = new MiniExcelReader::ExcelFile();
	if (!xExcel->open(strFile.c_str()))
	{
		std::cout << "can't open" << strFile << std::endl;
		return false;
	}

	// 获得sheet，就是表格下面一栏的标签栏
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
		loadDataAndProcessProperty(sheet, pClassData);
		loadIniData(sheet, pClassData);
	}
	else if (strSheetName.find("componen") != std::string::npos)
	{
		loadDataAndProcessComponent(sheet, pClassData);
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
		if (cell)
		{
			PropertyIndex[cell->value] = c;
		}
	}

	for (int r = dim.firstRow + PROPERTY_HEIGHT; r <= dim.lastRow; r++)
	{
		MiniExcelReader::Cell* pIDCell = sheet.getCell(r, dim.firstCol);
		if (pIDCell && !pIDCell->value.empty())
		{
			ClassElement::ElementData* pIniObject = new ClassElement::ElementData();
			pClassData->xIniData.xElementList[pIDCell->value] = pIniObject;

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

bool FileProcess::loadDataAndProcessProperty(MiniExcelReader::Sheet& sheet, ClassData* pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();
	std::string strSheetName = sheet.getName();
	transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

	std::map<std::string, int> descIndex;
	std::map<std::string, int> PropertyIndex;

	// 行，不读取第一行
	for (int r = dim.firstRow + 1; r <= dim.firstRow + PROPERTY_HEIGHT - 1; r++)
	{
		MiniExcelReader::Cell* cell = sheet.getCell(r, dim.firstCol);
		if (cell)
		{
			descIndex[cell->value] = r;
		}
	}

	// 列，不读取第一列
	for (int c = dim.firstCol + 1; c <= dim.lastCol; c++)
	{
		MiniExcelReader::Cell* cell = sheet.getCell(dim.firstRow, c);
		if (cell)
		{
			PropertyIndex[cell->value] = c;
		}
	}
	
	// 读取属性
	for (auto itProperty = PropertyIndex.begin(); itProperty != PropertyIndex.end(); ++itProperty)
	{
		std::string strPropertyName = itProperty->first;
		int nCol = itProperty->second;

		ClassProperty* pClassProperty = new ClassProperty();
		pClassData->xStructData.xPropertyList[strPropertyName] = pClassProperty;

		for (auto itDesc = descIndex.begin(); itDesc != descIndex.end(); ++itDesc)
		{
			std::string descName = itDesc->first;
			int nRow = itDesc->second;
		
			MiniExcelReader::Cell* pCell = sheet.getCell(nRow, nCol);
			if (pCell)
			{
				std::string descValue = pCell->value;

				pClassProperty->descList[descName] = descValue;
			}
		}
	}

	return false;
}

bool FileProcess::loadDataAndProcessComponent(MiniExcelReader::Sheet & sheet, ClassData * pClassData)
{
	const MiniExcelReader::Range& dim = sheet.getDimension();
	std::string strSheetName = sheet.getName();
	transform(strSheetName.begin(), strSheetName.end(), strSheetName.begin(), ::tolower);

	std::vector<std::string> colNames;
	for (int c = dim.firstCol; c <= dim.lastCol; c++)
	{
		MiniExcelReader::Cell* cell = sheet.getCell(dim.firstRow, c);
		if (cell)
		{
			colNames.push_back(cell->value);
		}
	}
	for (int r = dim.firstRow + 1; r <= dim.lastRow; r++)
	{
		std::string testValue = "";
		MiniExcelReader::Cell* cell = sheet.getCell(r, dim.firstCol);
		if (cell)
		{
			testValue = cell->value;
		}
		if (testValue == "")
		{
			continue;
		}
		//auto componentNode = structDoc.allocate_node(rapidxml::node_element, "Component", NULL);
		//componentNodes->append_node(componentNode);
		std::string strType = "";
		for (int c = dim.firstCol; c <= dim.lastCol; c++)
		{
			std::string name = colNames[c - 1];
			std::string value = "";
			MiniExcelReader::Cell* cell = sheet.getCell(r, c);
			if (cell)
			{
				std::string valueCell = cell->value;
				transform(valueCell.begin(), valueCell.end(), valueCell.begin(), ::toupper);
				if (valueCell == "TRUE" || valueCell == "FALSE")
				{
					value = valueCell == "TRUE" ? "1" : "0";
				}
				else
				{
					value = cell->value;
				}

				if (name == "Type")
				{
					strType = value;
				}

			}
			//componentNode->append_attribute(structDoc.allocate_attribute(NewChar(name), NewChar(value)));
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

		for (auto itElement = pClassDta->xIniData.xElementList.begin(); itElement != pClassDta->xIniData.xElementList.end(); ++itElement)
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
}
