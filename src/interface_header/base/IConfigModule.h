#pragma once


#include <exception>
#include <unordered_map>
#include "IModule.h"
#include "config_header/csv_parser.hpp"

namespace zq{


class IConfigModule : public IModule
{
public:

	using TypeInfoRef = std::reference_wrapper<const std::type_info>;
	struct Hasher
	{
		std::size_t operator()(TypeInfoRef code) const
		{
			return code.get().hash_code();
		}
	};
	struct EqualTo
	{
		bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const
		{
			return lhs.get() == rhs.get();
		}
	};

	using CsvMapType = std::unordered_map<TypeInfoRef, std::shared_ptr<CsvFile>, Hasher, EqualTo>;

	// 重建和加载配置文件，自动释放旧对象
	template<class T>
	bool create(const std::string& name)
	{
		std::string str = libManager_->getCfgCsvDir() + name;
		try
		{
			std::shared_ptr<TCsvFile<T>> pCsv = std::make_shared<TCsvFile<T>>();
			if (!pCsv->load(str))
			{
				throw std::string("can't open file : " + str);
				return false;
			}

			m_mapCsvFiles[typeid(TCsvFile<T>)] = std::static_pointer_cast<CsvFile>(pCsv);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "load csv error: file: " << str << ", errmsg: " << e.what();
			return false;
		}

		return true;
	}

	// 获取配置表
	template<class T>
	const std::shared_ptr<TCsvFile<T>> getCsv()
	{
		auto iter = m_mapCsvFiles.find(typeid(TCsvFile<T>));
		if (iter == m_mapCsvFiles.end())
		{
			return nullptr;
		}

		return std::static_pointer_cast<TCsvFile<T>>(iter->second);
	}

	// 获得指定的某行
	template<class T>
//	const std::shared_ptr<T> getCsvRow(const int id)
	const T* getCsvRow(const int id)
	{
		const std::shared_ptr<TCsvFile<T>> pCsvFile = getCsv<T>();
		if (pCsvFile == nullptr)
		{
			return nullptr;
		}
		return pCsvFile->getCsvRow(id);
	}

	// 获得所有的行
	template<class T>
	const typename TCsvFile<T>::CsvRowsMap* getCsvRowAll()
	{ 
		const std::shared_ptr<TCsvFile<T>> pCsvFile = getCsv<T>();
		if (pCsvFile == nullptr)
		{
			return nullptr;
		}

		return pCsvFile->getCsvRowAll();
	}

	// 一共的行数
	template<class T>
	const int getCsvRowNum(const int id)
	{
		const std::shared_ptr<TCsvFile<T>> pCsvFile = getCsv<T>();
		if (pCsvFile == nullptr)
		{
			return 0;
		}
		return pCsvFile->getCsvRowNum();
	}

protected:
	CsvMapType m_mapCsvFiles;
};

}
