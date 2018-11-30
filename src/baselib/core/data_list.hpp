#pragma once


#include "memory_counter.hpp"
#include "interface_header/base/uuid.h"
#include "interface_header/base/platform.h"
#include <variant>
#include <limits>

namespace zq{

enum EN_DATA_TYPE
{
    TDATA_UNKNOWN,  
	TDATA_INT64, 
	TDATA_DOUBLE,
    TDATA_STRING,   
    TDATA_GUID,
	TDATA_VECTOR2,
	TDATA_VECTOR3,
	TDATA_OBJECT,
    TDATA_MAX,
};

inline std::string dataType2Str(EN_DATA_TYPE type)
{
	switch (type)
	{
	case TDATA_UNKNOWN:	return "data_unknown";
	case TDATA_INT64: return "data_int64";
	case TDATA_DOUBLE:	return "data_double";
	case TDATA_STRING:	return "data_string";
	case TDATA_GUID:  return "data_guid";
	case TDATA_VECTOR2:	 return "data_vector2";
	case TDATA_VECTOR3:	return "data_vector3";
	case TDATA_OBJECT:	return "data_object";
	case TDATA_MAX:	 return "data_max";
	default:	return "data_null";
	}
}

inline bool invalidDataType(EN_DATA_TYPE type)
{
	if (type <= TDATA_UNKNOWN || type >= TDATA_MAX)
	{
		return  true;
	}

	return false;
}

inline bool isIntegralType(EN_DATA_TYPE type)
{
	return type == TDATA_INT64;
}

inline bool isFloatingType(EN_DATA_TYPE type)
{
	return type == TDATA_DOUBLE;
}

inline bool isStringType(EN_DATA_TYPE type)
{
	return type == TDATA_STRING;
}

inline bool isGuidType(EN_DATA_TYPE type)
{
	return type == TDATA_GUID;
}

const static std::string NULL_STR = "";
const static Guid NULL_GUID = Guid();
const static double VALID_FLOAT = std::numeric_limits<double>::max();
const static int64 VALID_INT = std::numeric_limits<int64>::max();


struct VariantData
{
	VariantData(EN_DATA_TYPE eType = TDATA_UNKNOWN)
	{
		type_ = eType;
		if (isIntegralType(type_))
		{
			variantData_ = VALID_INT;
		}
		else if (isFloatingType(type_))
		{
			variantData_ = VALID_FLOAT;
		}
		else if (isStringType(type_))
		{
			variantData_ = NULL_STR;
		}
		else if (type_ == TDATA_GUID)
		{
			variantData_ = NULL_GUID;
		}
	}

	VariantData(const VariantData& vd)
		:type_(vd.type_), variantData_(vd.variantData_)
	{
	}

	VariantData(VariantData&& vd)
		:type_(vd.type_), variantData_(std::move(vd.variantData_))
	{
	}

	VariantData& operator=(const VariantData& right)
	{
		if (this != &right)
		{
			type_ = right.type_;
			variantData_ = right.variantData_;
		}

		return *this;
	}

	VariantData& operator=(VariantData&& right)
	{
		if (this != &right)
		{
			type_ = right.type_;
			variantData_ = std::move(right.variantData_);
		}

		return *this;
	}

	bool operator==(const VariantData& right)
	{
		if (right.type_ == this->type_)
		{
			return variantData_ == variantData_;
		}

		return false;
	}

	bool operator!=(const VariantData& right)
	{
		if (right.type_ != this->type_)
		{
			return variantData_ != variantData_;
		}

		return false;
	}

	EN_DATA_TYPE type_;
	std::variant<int64, double, std::string, Guid> variantData_;
};

//class DataList :public MemoryCounter<DataList>
//{
//public:
//	DataList() : MemoryCounter(GET_CLASS_NAME(DataList))
//	{
//		mnUseSize = 0;
//		mvList.reserve(STACK_SIZE);
//		for (int i = 0; i < STACK_SIZE; ++i)
//		{
//			mvList.push_back(std::shared_ptr<VariantData>(new VariantData()));
//		}
//	}
//
//	virtual ~DataList()
//	{
//	}
//
//	virtual std::string toString(const int index) const
//	{
//
//		if (ValidIndex(index))
//		{
//			std::string strData;
//
//			const EN_DATA_TYPE eType = Type(index);
//			switch (eType)
//			{
//			case TDATA_INT:
//				strData = lexical_cast<std::string> (Int(index));
//				break;
//
//			case TDATA_FLOAT:
//				strData = lexical_cast<std::string> (Float(index));
//				break;
//
//			case TDATA_STRING:
//				strData = String(index);
//				break;
//
//			case TDATA_OBJECT:
//				strData = Object(index).toString();
//				break;
//			default:
//				strData = NULL_STR;
//				break;
//			}
//		}
//
//		return NULL_STR;
//	}
//
//	virtual bool toString(std::string& str, const std::string& strSplit) const
//	{
//		for (int i = 0; i < GetCount(); ++i)
//		{
//			std::string strVal = toString(i);
//			str += strVal;
//			str += strSplit;
//		}
//
//		std::string strTempSplit(strSplit);
//		std::string::size_type nPos = str.rfind(strSplit);
//		if (nPos == str.length() - strTempSplit.length())
//		{
//			str = str.substr(0, nPos);
//		}
//
//		return true;
//	}
//
//public:
//
//	virtual const std::shared_ptr<VariantData> GetStack(const int index) const
//	{
//		if (index < (int)mvList.size())
//		{
//			return mvList[index];
//		}
//
//		return std::shared_ptr<VariantData>();
//	}
//
//	virtual bool Concat(const DataList& src)
//	{
//		InnerAppendEx(src, 0, src.GetCount());
//		return true;
//	}
//
//	virtual bool Append(const DataList& src)
//	{
//		return Append(src, 0, src.GetCount());
//	}
//
//	virtual bool Append(const DataList& src, const int start, const int count)
//	{
//		if (start >= src.GetCount())
//		{
//			return false;
//		}
//
//		int end = start + count;
//
//		if (end > src.GetCount())
//		{
//			return false;
//		}
//
//		InnerAppendEx(src, start, end);
//
//		return true;
//	}
//
//	virtual bool Append(const VariantData& xData)
//	{
//		if (xData.getType() <= TDATA_UNKNOWN
//			|| xData.getType() >= TDATA_MAX)
//		{
//			return false;
//		}
//
//		switch (xData.getType())
//		{
//		case TDATA_INT:
//			AddInt(xData.getInt());
//			break;
//		case TDATA_FLOAT:
//			AddFloat(xData.getDouble());
//			break;
//		case TDATA_OBJECT:
//			AddObject(xData.getObject());
//			break;
//		case TDATA_STRING:
//			AddString(xData.getString());
//			break;
//		default:
//			break;
//		}
//
//		return true;
//	}
//
//	virtual void clear()
//	{
//		mnUseSize = 0;
//
//		if (mvList.size() > STACK_SIZE)
//		{
//			mvList.erase(mvList.begin() + STACK_SIZE, mvList.end());
//		}
//		for (int i = 0; i < STACK_SIZE; ++i)
//		{
//			mvList[i]->Reset();
//		}
//	}
//
//	virtual bool IsEmpty() const
//	{
//		return (0 == mnUseSize);
//	}
//
//	virtual int GetCount() const
//	{
//		return mnUseSize;
//	}
//
//	virtual EN_DATA_TYPE Type(const int index) const
//	{
//		if (!ValidIndex(index))
//		{
//			return TDATA_UNKNOWN;
//		}
//
//		if (index < STACK_SIZE)
//		{
//			return mvList[index]->getType();
//		}
//		else
//		{
//			const std::shared_ptr<VariantData> pData = GetStack(index);
//			if (pData)
//			{
//				return pData->getType();
//			}
//		}
//
//		return TDATA_UNKNOWN;
//	}
//
//	virtual bool TypeEx(const  int nType, ...) const
//	{
//
//		bool bRet = true;
//
//		if (TDATA_UNKNOWN == nType)
//		{
//			bRet = false;
//			return bRet;
//		}
//
//		EN_DATA_TYPE pareType = (EN_DATA_TYPE)nType;
//		va_list arg_ptr;
//		va_start(arg_ptr, nType);
//		int index = 0;
//
//		while (pareType != TDATA_UNKNOWN)
//		{
//
//			EN_DATA_TYPE varType = Type(index);
//			if (varType != pareType)
//			{
//				bRet = false;
//				break;
//			}
//
//			++index;
//			pareType = (EN_DATA_TYPE)va_arg(arg_ptr, int);
//		}
//
//		va_end(arg_ptr);
//
//		return bRet;
//	}
//
//	virtual bool Split(const std::string& str, const std::string& strSplit)
//	{
//
//		clear();
//
//		std::string strData(str);
//		if (strData.empty())
//		{
//			return true;
//		}
//
//		std::string temstrSplit(strSplit);
//		std::string::size_type pos;
//		strData += temstrSplit;
//		std::string::size_type size = strData.length();
//
//		for (std::string::size_type i = 0; i < size; i++)
//		{
//			pos = int(strData.find(temstrSplit, i));
//			if (pos < size)
//			{
//				std::string strSub = strData.substr(i, pos - i);
//				add(strSub.c_str());
//
//				i = pos + temstrSplit.size() - 1;
//			}
//		}
//
//		return true;
//	}
//
//
//	virtual bool add(const int64 value)
//	{
//		if (GetCount() == (int)(mvList.size()))
//		{
//			AddStatck();
//		}
//
//		std::shared_ptr<VariantData> var = GetStack(GetCount());
//		if (var)
//		{
//			var->setInt(value);
//			mnUseSize++;
//
//			return true;
//		}
//
//		return false;
//	}
//
//	virtual bool add(const double value)
//	{
//		if (GetCount() == (int)(mvList.size()))
//		{
//			AddStatck();
//		}
//
//		std::shared_ptr<VariantData> var = GetStack(GetCount());
//		if (var)
//		{
//			var->setDouble(value);
//			mnUseSize++;
//
//			return true;
//		}
//
//		return false;
//	}
//
//	virtual bool add(const std::string& value)
//	{
//		if (GetCount() == (int)(mvList.size()))
//		{
//			AddStatck();
//		}
//
//		std::shared_ptr<VariantData> var = GetStack(GetCount());
//		if (var)
//		{
//			var->setString(value);
//			mnUseSize++;
//
//			return true;
//		}
//
//		return false;
//	}
//
//	virtual bool add(const Guid& value)
//	{
//		if (GetCount() == (int)(mvList.size()))
//		{
//			AddStatck();
//		}
//
//		std::shared_ptr<VariantData> var = GetStack(GetCount());
//		if (var)
//		{
//			var->setObject(value);
//			mnUseSize++;
//
//			return true;
//		}
//
//		return false;
//	}
//
//	virtual bool Set(const int index, const int64 value)
//	{
//		if (ValidIndex(index) && Type(index) == TDATA_INT)
//		{
//			std::shared_ptr<VariantData> var = GetStack(index);
//			if (var)
//			{
//				var->setInt(value);
//
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//	virtual bool Set(const int index, const double value)
//	{
//		if (ValidIndex(index) && Type(index) == TDATA_FLOAT)
//		{
//			std::shared_ptr<VariantData> var = GetStack(index);
//			if (var)
//			{
//				var->setDouble(value);
//
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//	virtual bool Set(const int index, const std::string& value)
//	{
//		if (ValidIndex(index) && Type(index) == TDATA_STRING)
//		{
//			std::shared_ptr<VariantData> var = GetStack(index);
//			if (var)
//			{
//				var->setString(value);
//
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//	virtual bool Set(const int index, const Guid& value)
//	{
//		if (ValidIndex(index) && Type(index) == TDATA_OBJECT)
//		{
//			std::shared_ptr<VariantData> var = GetStack(index);
//			if (var)
//			{
//				var->setObject(value);
//
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//	virtual int64 Int(const int index) const
//	{
//		if (ValidIndex(index))
//		{
//			if (Type(index) == TDATA_INT)
//			{
//				const std::shared_ptr<VariantData> var = GetStack(index);
//				return var->getInt();
//			}
//		}
//
//		return 0;
//	}
//
//	inline int Int32(const int index) const
//	{
//		return (int)Int(index);
//	}
//
//	virtual double Float(const int index) const
//	{
//		if (ValidIndex(index))
//		{
//			const std::shared_ptr<VariantData> var = mvList[index];
//			if (var && TDATA_FLOAT == var->getType())
//			{
//				return var->getDouble();
//			}
//		}
//
//		return 0.0f;
//	}
//
//	virtual const std::string& String(const int index) const
//	{
//		if (ValidIndex(index))
//		{
//			const std::shared_ptr<VariantData> var = mvList[index];
//			if (var && TDATA_STRING == var->getType())
//			{
//				return var->getString();
//			}
//		}
//
//		return NULL_STR;
//	}
//
//	virtual const Guid& Object(const int index) const
//	{
//		if (ValidIndex(index))
//		{
//			EN_DATA_TYPE type = Type(index);
//			if (TDATA_OBJECT == type)
//			{
//				std::shared_ptr<VariantData> var = GetStack(index);
//				if (var)
//				{
//					return var->getObject();
//				}
//			}
//		}
//
//		return NULL_OBJECT;
//	}
//
//	bool AddInt(const int64 value)
//	{
//		return add(value);
//	}
//	bool AddFloat(const double value)
//	{
//		return add(value);
//	}
//	bool AddString(const std::string& value)
//	{
//		return add(value);
//	}
//	bool AddStringFromChar(const char* value)
//	{
//		return add(value);
//	}
//	bool AddObject(const Guid& value)
//	{
//		return add(value);
//	}
//	bool setInt(const int index, const int64 value)
//	{
//		return Set(index, value);
//	}
//	bool setDouble(const int index, const double value)
//	{
//		return Set(index, value);
//	}
//	bool setString(const int index, const std::string& value)
//	{
//		return Set(index, value);
//	}
//	bool setObject(const int index, const Guid& value)
//	{
//		return Set(index, value);
//	}
//	inline bool Compare(const int nPos, const DataList& src) const
//	{
//		if (src.GetCount() > nPos
//			&& GetCount() > nPos
//			&& src.Type(nPos) == Type(nPos))
//		{
//			switch (src.Type(nPos))
//			{
//			case TDATA_INT:
//				return Int(nPos) == src.Int(nPos);
//				break;
//
//			case TDATA_FLOAT:
//				return fabs(Float(nPos) - src.Float(nPos)) < 0.001f;
//				break;
//
//			case TDATA_STRING:
//				return String(nPos) == src.String(nPos);
//				break;
//
//			case TDATA_OBJECT:
//				return Object(nPos) == src.Object(nPos);
//				break;
//
//			default:
//				return false;
//				break;
//			}
//		}
//
//		return false;
//	}
//
//
//	inline bool operator==(const DataList& src) const
//	{
//		if (src.GetCount() == GetCount())
//		{
//			for (int i = 0; i < GetCount(); i++)
//			{
//				if (!Compare(i, src))
//				{
//					return false;
//				}
//			}
//
//			return true;
//		}
//
//		return false;
//	}
//
//	inline bool operator!=(const DataList& src)
//	{
//		return !(*this == src);
//	}
//	inline DataList& operator<<(const double value)
//	{
//		add(value);
//		return *this;
//	}
//	inline DataList& operator<<(const char* value)
//	{
//		add(value);
//		return *this;
//	}
//	inline DataList& operator<<(const std::string& value)
//	{
//		add(value);
//		return *this;
//	}
//
//	inline DataList& operator<<(const int64& value)
//	{
//		add(value);
//		return *this;
//	}
//	inline DataList& operator<<(const int value)
//	{
//		add((int64)value);
//		return *this;
//	}
//	inline DataList& operator<<(const Guid& value)
//	{
//		add(value);
//		return *this;
//	}
//	inline DataList& operator<<(const DataList& value)
//	{
//		Concat(value);
//		return *this;
//	}
//	enum { STACK_SIZE = 8 };
//
//protected:
//
//	bool ValidIndex(int index) const
//	{
//		return (index < GetCount()) && (index >= 0);
//	}
//
//	void AddStatck()
//	{
//		for (int i = 0; i < STACK_SIZE; ++i)
//		{
//			std::shared_ptr<VariantData> pData(new VariantData());
//			mvList.push_back(pData);
//		}
//	}
//
//	void InnerAppendEx(const DataList& src, const int start, const int end)
//	{
//		for (int i = start; i < end; ++i)
//		{
//			EN_DATA_TYPE vType = src.Type(i);
//			switch (vType)
//			{
//			case TDATA_INT:
//				AddInt(src.Int(i));
//				break;
//			case TDATA_FLOAT:
//				AddFloat(src.Float(i));
//				break;
//			case TDATA_STRING:
//				AddString(src.String(i));
//				break;
//			case TDATA_OBJECT:
//				AddObject(src.Object(i));
//				break;
//			default:
//				break;
//			}
//		}
//	}
//protected:
//
//	std::vector<VariantData> vecVData_;;
//};


}