#pragma once

#include <cstdarg>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>
#include <map>
#include <typeinfo>
#include <iomanip>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include "memory_counter.hpp"
#include "interface_header/uuid.h"
#include "interface_header/platform.h"
#include "baselib/base_code/variant.hpp"

namespace zq{

enum EN_DATA_TYPE
{
    TDATA_UNKNOWN,  
    TDATA_INT,      
    TDATA_FLOAT,    
    TDATA_STRING,   
    TDATA_OBJECT,   
	TDATA_VECTOR2,
	TDATA_VECTOR3,
    TDATA_MAX,
};

const static std::string NULL_STR = "";
const static Guid NULL_OBJECT = Guid();
const static double NULL_FLOAT = 0.0;
const static int64 NULL_INT = 0;

struct AbstractData
{
public:
	AbstractData()
	{
		nType = TDATA_UNKNOWN;
	}

	AbstractData(EN_DATA_TYPE eType)
	{
		nType = eType;
	}

	AbstractData(const AbstractData& value)
	{
		nType = value.nType;
		variantData = value.variantData;
	}

	~AbstractData()
	{
		nType = TDATA_UNKNOWN;
	}

	inline bool operator==(const AbstractData& src) const
	{
		//&& src.variantData == variantData
		if (src.getType() == getType())
		{
			switch (getType())
			{
			case TDATA_INT:
			{
				if (src.getInt() == getInt())
				{
					return true;
				}
			}
			break;
			case TDATA_FLOAT:
			{
				double fValue = getFloat() - src.getFloat();
				if (fValue < 0.001  && fValue > -0.001)
				{
					return true;
				}
			}
			break;
			case TDATA_STRING:
			{
				if (src.getString() == getString())
				{
					return true;
				}
			}
			break;
			case TDATA_OBJECT:
			{
				if (src.getObject() == getObject())
				{
					return true;
				}
			}
			break;
			default:
				break;
			}
		}

		return false;
	}

	void Reset()
	{
		variantData = mapbox::util::variant<int64, double, std::string, Guid>();
		nType = TDATA_UNKNOWN;
	}

	bool IsNullValue() const
	{
		bool bChanged = false;

		switch (getType())
		{
		case TDATA_INT:
		{
			if (0 != getInt())
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_FLOAT:
		{
			double fValue = getFloat();
			if (fValue > 0.001 || fValue < -0.001)
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_STRING:
		{
			const std::string& strData = getString();
			if (!strData.empty())
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_OBJECT:
		{
			if (!getObject().isNull())
			{
				bChanged = true;
			}
		}
		break;
		default:
			break;
		}

		return !bChanged;
	}

	EN_DATA_TYPE getType() const	  
	{
		return nType;
	}

	void setInt(const int64 var)
	{
		if (nType == TDATA_INT || TDATA_UNKNOWN == nType)
		{
			nType = TDATA_INT;
			variantData = (int64)var;
		}
	}

	void setFloat(const double var)
	{
		if (nType == TDATA_FLOAT || TDATA_UNKNOWN == nType)
		{
			nType = TDATA_FLOAT;
			variantData = (double)var;
		}
	}

	void setString(const std::string& var)
	{
		if (nType == TDATA_STRING || TDATA_UNKNOWN == nType)
		{
			nType = TDATA_STRING;
			variantData = (std::string)var;
		}
	}

	void setObject(const Guid var)
	{
		if (nType == TDATA_OBJECT || TDATA_UNKNOWN == nType)
		{
			nType = TDATA_OBJECT;
			variantData = (Guid)var;
		}
	}

	int64 getInt() const
	{
		if (TDATA_INT == nType)
		{
			//return boost::get<int64>(variantData);
			return variantData.get<int64>();
		}

		return NULL_INT;
	}

	double getFloat() const
	{
		if (TDATA_FLOAT == nType)
		{
			//return boost::get<double>(variantData);
			return variantData.get<double>();
		}

		return NULL_FLOAT;
	}
	const std::string& getString() const
	{
		if (TDATA_STRING == nType)
		{
			//return boost::get<const std::string&>(variantData);
			return variantData.get<std::string>();
		}

		return NULL_STR;
	}

	const char* GetCharArr() const
	{
		if (TDATA_STRING == nType)
		{
			//return boost::get<const std::string&>(variantData);
			return variantData.get<std::string>().c_str();
		}

		return NULL_STR.c_str();
	}

	const Guid& getObject() const
	{
		if (TDATA_OBJECT == nType)
		{
			//return boost::get<const Guid&>(variantData);
			return variantData.get<Guid>();
		}

		return NULL_OBJECT;
	}

	std::string toString() const
	{
		std::string strData;

		switch (nType)
		{
		case TDATA_INT:
			strData = lexical_cast<std::string> (getInt());
			break;

		case TDATA_FLOAT:
			strData = lexical_cast<std::string> (getFloat());
			break;

		case TDATA_STRING:
			strData = getString();
			break;

		case TDATA_OBJECT:
			strData = getObject().toString();
			break;

		default:
			strData = NULL_STR;
			break;
		}
		return strData;
	}

	bool fromString(const std::string& strData)
	{
		try
		{
			switch (nType)
			{
			case TDATA_INT:
				setInt(lexical_cast<int64> (strData));
				break;

			case TDATA_FLOAT:
				setFloat(lexical_cast<float> (strData));
				break;

			case TDATA_STRING:
				setString(strData);
				break;

			case TDATA_OBJECT:
			{
				Guid xID;
				xID.fromString(strData);
				setObject(xID);
			}
			break;	
			default:
				break;
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
		
		return false;
	}

private:

	EN_DATA_TYPE nType;

public:
	//std::variant
	mapbox::util::variant<int64, double, std::string, Guid> variantData;
};

class DataList :public MemoryCounter<DataList>
{
public:
    DataList() : MemoryCounter(GET_CLASS_NAME(DataList))
    {
        mnUseSize = 0;
        mvList.reserve(STACK_SIZE);
        for (int i = 0; i < STACK_SIZE; ++i)
        {
            mvList.push_back(std::shared_ptr<AbstractData>(new AbstractData()));
        }
    }

	virtual ~DataList()
	{
	}

	virtual std::string toString(const int index) const
	{

		if (ValidIndex(index))
		{
			std::string strData;

			const EN_DATA_TYPE eType = Type(index);
			switch (eType)
			{
			case TDATA_INT:
				strData = lexical_cast<std::string> (Int(index));
				break;

			case TDATA_FLOAT:
				strData = lexical_cast<std::string> (Float(index));
				break;

			case TDATA_STRING:
				strData = String(index);
				break;

			case TDATA_OBJECT:
				strData = Object(index).toString();
				break;
			default:
				strData = NULL_STR;
				break;
			}
		}

		return NULL_STR;
	}

	virtual bool toString(std::string& str, const std::string& strSplit) const
	{
		for (int i = 0; i < GetCount(); ++i)
		{
			std::string strVal = toString(i);
			str += strVal;
			str += strSplit;
		}

		std::string strTempSplit(strSplit);
		std::string::size_type nPos = str.rfind(strSplit);
		if (nPos == str.length() - strTempSplit.length())
		{
			str = str.substr(0, nPos);
		}

		return true;
	}

public:

	virtual const std::shared_ptr<AbstractData> GetStack(const int index) const
	{
		if (index < (int)mvList.size())
		{
			return mvList[index];
		}

		return std::shared_ptr<AbstractData>();
	}

	virtual bool Concat(const DataList& src)
	{
		InnerAppendEx(src, 0, src.GetCount());
		return true;
	}
    
	virtual bool Append(const DataList& src)
	{
		return Append(src, 0, src.GetCount());
	}

	virtual bool Append(const DataList& src, const int start, const int count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		int end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}

		InnerAppendEx(src, start, end);

		return true;
	}
    
	virtual bool Append(const AbstractData& xData)
	{
		if (xData.getType() <= TDATA_UNKNOWN
			|| xData.getType() >= TDATA_MAX)
		{
			return false;
		}

		switch (xData.getType())
		{
		case TDATA_INT:
			AddInt(xData.getInt());
			break;
		case TDATA_FLOAT:
			AddFloat(xData.getFloat());
			break;
		case TDATA_OBJECT:
			AddObject(xData.getObject());
			break;
		case TDATA_STRING:
			AddString(xData.getString());
			break;
		default:
			break;
		}

		return true;
	}
    
	virtual void clear()
	{
		mnUseSize = 0;

		if (mvList.size() > STACK_SIZE)
		{
			mvList.erase(mvList.begin() + STACK_SIZE, mvList.end());
		}
		for (int i = 0; i < STACK_SIZE; ++i)
		{
			mvList[i]->Reset();
		}
	}
    
	virtual bool IsEmpty() const
	{
		return (0 == mnUseSize);
	}
    
	virtual int GetCount() const
	{
		return mnUseSize;
	}
    
	virtual EN_DATA_TYPE Type(const int index) const
	{
		if (!ValidIndex(index))
		{
			return TDATA_UNKNOWN;
		}

		if (index < STACK_SIZE)
		{
			return mvList[index]->getType();
		}
		else
		{
			const std::shared_ptr<AbstractData> pData = GetStack(index);
			if (pData)
			{
				return pData->getType();
			}
		}

		return TDATA_UNKNOWN;
	}
    
	virtual bool TypeEx(const  int nType, ...) const
	{

		bool bRet = true;

		if (TDATA_UNKNOWN == nType)
		{
			bRet = false;
			return bRet;
		}

		EN_DATA_TYPE pareType = (EN_DATA_TYPE)nType;
		va_list arg_ptr;
		va_start(arg_ptr, nType);
		int index = 0;

		while (pareType != TDATA_UNKNOWN)
		{

			EN_DATA_TYPE varType = Type(index);
			if (varType != pareType)
			{
				bRet = false;
				break;
			}

			++index;
			pareType = (EN_DATA_TYPE)va_arg(arg_ptr, int);
		}

		va_end(arg_ptr);

		return bRet;
	}
    
	virtual bool Split(const std::string& str, const std::string& strSplit)
	{

		clear();

		std::string strData(str);
		if (strData.empty())
		{
			return true;
		}

		std::string temstrSplit(strSplit);
		std::string::size_type pos;
		strData += temstrSplit;
		std::string::size_type size = strData.length();

		for (std::string::size_type i = 0; i < size; i++)
		{
			pos = int(strData.find(temstrSplit, i));
			if (pos < size)
			{
				std::string strSub = strData.substr(i, pos - i);
				add(strSub.c_str());

				i = pos + temstrSplit.size() - 1;
			}
		}

		return true;
	}

    
	virtual bool add(const int64 value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		std::shared_ptr<AbstractData> var = GetStack(GetCount());
		if (var)
		{
			var->setInt(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool add(const double value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		std::shared_ptr<AbstractData> var = GetStack(GetCount());
		if (var)
		{
			var->setFloat(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool add(const std::string& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		std::shared_ptr<AbstractData> var = GetStack(GetCount());
		if (var)
		{
			var->setString(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool add(const Guid& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		std::shared_ptr<AbstractData> var = GetStack(GetCount());
		if (var)
		{
			var->setObject(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Set(const int index, const int64 value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_INT)
		{
			std::shared_ptr<AbstractData> var = GetStack(index);
			if (var)
			{
				var->setInt(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const double value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_FLOAT)
		{
			std::shared_ptr<AbstractData> var = GetStack(index);
			if (var)
			{
				var->setFloat(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const std::string& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_STRING)
		{
			std::shared_ptr<AbstractData> var = GetStack(index);
			if (var)
			{
				var->setString(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const Guid& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_OBJECT)
		{
			std::shared_ptr<AbstractData> var = GetStack(index);
			if (var)
			{
				var->setObject(value);

				return true;
			}
		}

		return false;
	}
    
	virtual int64 Int(const int index) const
	{
		if (ValidIndex(index))
		{
			if (Type(index) == TDATA_INT)
			{
				const std::shared_ptr<AbstractData> var = GetStack(index);
				return var->getInt();
			}
		}

		return 0;
	}

	inline int Int32(const int index) const
	{
		return (int) Int(index);
	}

	virtual double Float(const int index) const
	{
		if (ValidIndex(index))
		{
			const std::shared_ptr<AbstractData> var = mvList[index];
			if (var && TDATA_FLOAT == var->getType())
			{
				return var->getFloat();
			}
		}

		return 0.0f;
	}

	virtual const std::string& String(const int index) const
	{
		if (ValidIndex(index))
		{
			const std::shared_ptr<AbstractData> var = mvList[index];
			if (var && TDATA_STRING == var->getType())
			{
				return var->getString();
			}
		}

		return NULL_STR;
	}

	virtual const Guid& Object(const int index) const
	{
		if (ValidIndex(index))
		{
			EN_DATA_TYPE type = Type(index);
			if (TDATA_OBJECT == type)
			{
				std::shared_ptr<AbstractData> var = GetStack(index);
				if (var)
				{
					return var->getObject();
				}
			}
		}

		return NULL_OBJECT;
	}

    bool AddInt(const int64 value)
    {
        return add(value);
    }
    bool AddFloat(const double value)
    {
        return add(value);
    }
    bool AddString(const std::string& value)
    {
        return add(value);
    }
    bool AddStringFromChar(const char* value)
    {
        return add(value);
    }
    bool AddObject(const Guid& value)
    {
        return add(value);
    }
    bool setInt(const int index, const int64 value)
    {
        return Set(index, value);
    }
    bool setFloat(const int index, const double value)
    {
        return Set(index, value);
    }
    bool setString(const int index, const std::string& value)
    {
        return Set(index, value);
    }
    bool setObject(const int index, const Guid& value)
    {
        return Set(index, value);
    }
    inline bool Compare(const int nPos, const DataList& src) const
    {
        if (src.GetCount() > nPos
            && GetCount() > nPos
            && src.Type(nPos) == Type(nPos))
        {
            switch (src.Type(nPos))
            {
                case TDATA_INT:
                    return Int(nPos) == src.Int(nPos);
                    break;

                case TDATA_FLOAT:
                    return fabs(Float(nPos) - src.Float(nPos)) < 0.001f;
                    break;

                case TDATA_STRING:
                    return String(nPos) == src.String(nPos);
                    break;

                case TDATA_OBJECT:
                    return Object(nPos) == src.Object(nPos);
                    break;

                default:
                    return false;
                    break;
            }
        }

        return false;
    }


    inline bool operator==(const DataList& src) const
    {
        if (src.GetCount() == GetCount())
        {
            for (int i = 0; i < GetCount(); i++)
            {
                if (!Compare(i, src))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    inline bool operator!=(const DataList& src)
    {
        return !(*this == src);
    }
    inline DataList& operator<<(const double value)
    {
        add(value);
        return *this;
    }
    inline DataList& operator<<(const char* value)
    {
        add(value);
        return *this;
    }
    inline DataList& operator<<(const std::string& value)
    {
        add(value);
        return *this;
    }

    inline DataList& operator<<(const int64& value)
    {
        add(value);
        return *this;
    }
    inline DataList& operator<<(const int value)
    {
        add((int64)value);
        return *this;
    }
    inline DataList& operator<<(const Guid& value)
    {
        add(value);
        return *this;
    }
    inline DataList& operator<<(const DataList& value)
    {
        Concat(value);
        return *this;
    }
    enum { STACK_SIZE = 8 };

protected:

	bool ValidIndex(int index) const
	{
		return (index < GetCount()) && (index >= 0);
	}

	void AddStatck()
	{
		for (int i = 0; i < STACK_SIZE; ++i)
		{
			std::shared_ptr<AbstractData> pData(new AbstractData());
			mvList.push_back(pData);
		}
	}

	void InnerAppendEx(const DataList& src, const int start, const int end)
	{
		for (int i = start; i < end; ++i)
		{
			EN_DATA_TYPE vType = src.Type(i);
			switch (vType)
			{
			case TDATA_INT:
				AddInt(src.Int(i));
				break;
			case TDATA_FLOAT:
				AddFloat(src.Float(i));
				break;
			case TDATA_STRING:
				AddString(src.String(i));
				break;
			case TDATA_OBJECT:
				AddObject(src.Object(i));
				break;
			default:
				break;
			}
		}
	}
protected:

    int mnUseSize;
    std::vector< std::shared_ptr<AbstractData> > mvList;
    std::map<std::string, std::shared_ptr<AbstractData> > mxMap;
};

const static AbstractData NULL_TDATA = AbstractData();

}