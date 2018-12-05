#pragma once


#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <map>


namespace zq {

// 空类型，用作调用T类型代理，靠编译期防止进入代理递归死循环
class Proxy{};

// 解析过程中用到的跳跃器
static struct Skip { Skip(int) {} } _Skip(0);

class CsvParse
{
public:	
	CsvParse(std::string strRow, char cDelim)
	{
		m_listFields.clear();
		std::istringstream sin(strRow); 
		std::string strField;
		while(std::getline(sin, strField, cDelim))
		{
			m_listFields.push_back(strField); 
		}
	}

	// 操作符重载反向代理
	template<class T>
	CsvParse& operator >> (T& out)
	{
		if(m_listFields.size() <= 0)
		{
			throw 1; // 字段不够还在尝试解析
		}

		this->operator >><Proxy>(out);
		m_listFields.pop_front();

		return *this;
	}

	template<class T>
	T pop()
	{
		T t;
		this->operator >>(t);
		return t;
	}

	// 解析跳过一项
	template<typename _Proxy>
	void operator >> (Skip& skip) {}

	// bool
	template<typename _Proxy>
	void operator >> (bool& bOut)
	{
		bOut = std::stoi(m_listFields.front()) > 0 ? true : false;;
	}

	// short
	template<typename _Proxy>
	void operator >> (short& nOut)
	{
		nOut = (short)std::stoi(m_listFields.front());
	}

	// int
	template<typename _Proxy>
	void operator >> (int& nOut)
	{
		nOut = std::stoi(m_listFields.front());
	}

	// long
	template<typename _Proxy>
	void operator >> (long& nOut)
	{
		nOut = std::stol(m_listFields.front());
	}

	// unsigned long
	template<typename _Proxy>
	void operator >> (unsigned long& nOut)
	{
		nOut = std::stoul(m_listFields.front());
	}

	// long long
	template<typename _Proxy>
	void operator >> (long long& nOut)
	{
		nOut = std::stoll(m_listFields.front());
	}
	
	// unsigned long long
	template<typename _Proxy>
	void operator >> (unsigned long long& nOut)
	{
		nOut = std::stoull(m_listFields.front());
	}
	
	// float
	template<typename _Proxy>
	void operator >> (float& nOut)
	{
		nOut = std::stof(m_listFields.front());
	}
	
	// double
	template<typename _Proxy>
	void operator >> (double& nOut)
	{
		nOut = std::stod(m_listFields.front());
	}
	
	// long double
	template<typename _Proxy>
	void operator >> (long double& nOut)
	{
		nOut = std::stold(m_listFields.front());
	}

	// std::string
	template<typename _Proxy>
	void operator >> (std::string& strOut)
	{
		strOut = m_listFields.front();
	}

	// char
	template<typename _Proxy>
	void operator >> (char& chOut)
	{
		chOut = m_listFields.front().c_str()[0];
	}

	// 取key，一般认为csv每行首位是key
	int parseKey()
	{
		int id = -1;
		if(m_listFields.size() > 0)
		{
			this->operator >><Proxy>(id);
		}

		return id;
	}

	std::deque<std::string>& getFields()
	{
		return m_listFields;
	}

	std::deque<std::string>&& moveFields()
	{
		return std::move(m_listFields);
	}

private:
	std::deque<std::string> m_listFields;
};


class CsvFile
{
public:
	CsvFile() {}
	CsvFile(const CsvFile&) = default;
	CsvFile& operator = (CsvFile&) = default;
	virtual ~CsvFile(void) {}
	
	bool load(const std::string& strFileName, char cDelim = ',')
	{
		std::ifstream istreamReader(strFileName);
		if(istreamReader.is_open() == false)
		{
			return false;
		}

		std::string strRow;

		// 字段
		std::getline(istreamReader, strRow);
		parseTitle(CsvParse(strRow, cDelim));

		// 类型
		std::getline(istreamReader, strRow);
		parseTypes(CsvParse(strRow, cDelim));

		// 字段的描述
		std::getline(istreamReader, strRow);
		parseDesc(CsvParse(strRow, cDelim));

		//{
		//	std::istringstream sin(strRow);
		//	std::string strCol;
		//	while (std::getline(sin, strCol, cDelim))
		//	{
		//		m_listCol.push_back(strCol);
		//	}
		//}

		while (std::getline(istreamReader, strRow))
		{
			if(parseRowProxy(CsvParse(strRow, cDelim)) == false)
			{
				return false;
			}
		}

		return true;
	}

protected:
	virtual void parseTitle(CsvParse&& parse) = 0;
	virtual void parseTypes(CsvParse&& parse) = 0;
	virtual void parseDesc(CsvParse&& parse) = 0;
	virtual void parseRow(CsvParse&& parse) = 0;
	virtual bool parseRowProxy(CsvParse&& parse) = 0;

protected:
	std::deque<std::string> m_listCol;
};



template<class T>
class TCsvFile : public CsvFile
{
public:
	using CsvRowsMap = std::map<int, T>;

	TCsvFile<T>() = default;
	virtual ~TCsvFile<T>()
	{
		m_mapData.clear();
	}

	const CsvRowsMap* getCsvRowAll() const { return &m_mapData; }

//	const std::shared_ptr<T> getCsvRow(const int id)
	const T* getCsvRow(const int id)
	{
		auto it = m_mapData.find(id);
		if(it == m_mapData.end())
		{
			return nullptr;
		}
		return &it->second;
	}

	const int getCsvRowNum()
	{
		return m_mapData.size();
	}

	const std::map<std::string, std::string> *getCsvCol(const int id)
	{
		auto iter =  m_mapCol.find(id);
		if (iter == m_mapCol.end())
		{
			return nullptr;
		}
		return &iter->second;
	}

protected:

	virtual void parseTitle(CsvParse&& parse) override
	{
		title_ = std::move(parse.moveFields());
	}

	virtual void parseTypes(CsvParse&& parse) override
	{
		strType_ = std::move(parse.moveFields());
	}

	virtual void parseDesc(CsvParse&& parse) override
	{
		strDesc_ = std::move(parse.moveFields());
	}

	virtual bool parseRowProxy(CsvParse&& parse) override
	{
		int id = parse.parseKey();
		if (id != -1)
		{
			std::map<std::string, std::string> mapColItem;
			std::deque<std::string> listFields = parse.getFields();

			for (auto &it : m_listCol)
			{
				if (!listFields.empty())
				{
					mapColItem[it] = listFields.front();
					listFields.pop_front();
				}
				else
				{
					mapColItem[it];
				}
			}
			m_mapCol[id] = mapColItem;
			thisRow = &m_mapData[id];
			parseRow(std::move(parse));
		}

		return true;
	}

	virtual void parseRow(CsvParse&& parse);

public:
	T* thisRow;

protected:

	CsvRowsMap m_mapData; ;
	std::map<int, std::map<std::string, std::string>> m_mapCol;

	std::deque<std::string> title_;
	std::deque<std::string> strType_;
	std::deque<std::string> strDesc_;
};



} 