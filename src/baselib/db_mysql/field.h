#pragma once

#include "baselib/interface_header/platform.h"
#include "database_env_fwd.h"
#include <vector>

namespace zq {

enum class DatabaseFieldTypes : uint8
{
	Null,
	Int8,
	Int16,
	Int32,
	Int64,
	Float,
	Double,
	Decimal,
	Date,
	Binary
};

class Field
{
	friend class ResultSet;
	friend class PreparedResultSet;

public:

	// ×Ö¶ÎÊý¾Ý
#pragma pack(push, 1)
	struct FieldData
	{
		uint32 length;          // Length (prepared strings only)
		void* value;            // Actual data in memory
		DatabaseFieldTypes type;  // Field type
		bool raw;               // Raw bytes? (Prepared statement or ad hoc)
	} data;
#pragma pack(pop)

	Field()
	{
		data.value = nullptr;
		data.type = DatabaseFieldTypes::Null;
		data.length = 0;
		data.raw = false;
	}
	~Field()
	{
		cleanUp();
	}

	bool getBool() const 
	{
		return getUInt8() == 1 ? true : false;
	}

	uint8 getUInt8() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int8))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<uint8*>(data.value);
		}

		return static_cast<uint8>(strtoul((char*)data.value, nullptr, 10));
	}

	int8 getInt8() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int8))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<int8*>(data.value);
		}

		return static_cast<int8>(strtol((char*)data.value, NULL, 10));
	}

	uint16 getUInt16() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int16))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<uint16*>(data.value);
		}

		return static_cast<uint16>(strtoul((char*)data.value, nullptr, 10));
	}

	int16 getInt16() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int16))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<int16*>(data.value);
		}

		return static_cast<int16>(strtol((char*)data.value, NULL, 10));
	}

	uint32 getUInt32() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int32))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<uint32*>(data.value);
		}

		return static_cast<uint32>(strtoul((char*)data.value, nullptr, 10));
	}

	int32 getInt32() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int32))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<int32*>(data.value);
		}

		return static_cast<int32>(strtol((char*)data.value, nullptr, 10));
	}

	uint64 getUInt64() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int64))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<uint64*>(data.value);
		}

		return static_cast<uint64>(strtoull((char*)data.value, nullptr, 10));
	}

	int64 getInt64() const
	{
		if (!data.value)
		{
			return 0;
		}

		if (!isType(DatabaseFieldTypes::Int64))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0;
		}

		if (data.raw)
		{
			return *reinterpret_cast<int64*>(data.value);
		}

		return static_cast<int64>(strtoll((char*)data.value, NULL, 10));
	}

	float getFloat() const
	{
		if (!data.value)
		{
			return 0.0f;
		}

		if (!isType(DatabaseFieldTypes::Float))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0.0f;
		}

		if (data.raw)
		{
			return *reinterpret_cast<float*>(data.value);
		}

		return static_cast<float>(atof((char*)data.value));
	}

	double getDouble() const
	{
		if (!data.value)
		{
			return 0.0;
		}

		if (!isType(DatabaseFieldTypes::Double) && !isType(DatabaseFieldTypes::Decimal))
		{
			logWrongType(data.type, __FUNCTION__);
			return 0.0;
		}

		if (data.raw && !isType(DatabaseFieldTypes::Decimal))
		{
			return *reinterpret_cast<double*>(data.value);
		}

		return static_cast<double>(atof((char*)data.value));
	}

	char const* getCString() const
	{
		if (!data.value)
		{
			return nullptr;
		}

		if (isNumeric() && data.raw)
		{
			logWrongType(data.type, __FUNCTION__);
			return nullptr;
		}

		return static_cast<char const*>(data.value);
	}

	std::string getString() const
	{
		if (!data.value)
		{
			return "";
		}

		char const* string = getCString();
		if (!string)
		{
			return "";
		}

		return std::string(string, data.length);
	}

	std::vector<uint8>&& getBinary() const
	{
		std::vector<uint8> result;
		if (!data.value || !data.length)
		{
			return std::move(result);
		}

		result.resize(data.length);
		memcpy(result.data(), data.value, data.length);
		return std::move(result);
	}

	bool isNull() const
	{
		return data.value == nullptr;
	}

	void setByteValue(void* newValue, DatabaseFieldTypes newType, uint32 length)
	{
		// This value stores raw bytes that have to be explicitly cast later
		data.value = newValue;
		data.length = length;
		data.type = newType;
		data.raw = true;
	}

	void setStructuredValue(char* newValue, DatabaseFieldTypes newType, uint32 length)
	{
		if (data.value)
		{
			cleanUp();
		}

		// This value stores somewhat structured data that needs function style casting
		if (newValue)
		{
			data.value = new char[length + 1];
			memcpy(data.value, newValue, length);
			*(reinterpret_cast<char*>(data.value) + length) = '\0';
			data.length = length;
		}

		data.type = newType;
		data.raw = false;
	}

	void cleanUp()
	{
		// Field does not own the data if fetched with prepared statement
		if (!data.raw)
		{
			delete[]((char*)data.value);
		}

		data.value = nullptr;
	}

	bool isType(DatabaseFieldTypes type) const
	{
		return data.type == type;
	}

	bool isNumeric() const
	{
		return (data.type == DatabaseFieldTypes::Int8 ||
			data.type == DatabaseFieldTypes::Int16 ||
			data.type == DatabaseFieldTypes::Int32 ||
			data.type == DatabaseFieldTypes::Int64 ||
			data.type == DatabaseFieldTypes::Float ||
			data.type == DatabaseFieldTypes::Double);
	}

	void logWrongType(DatabaseFieldTypes datatype, const char* str) const
	{
		LOG_ERROR << "type error, datatype: " << (int)datatype << ", function:" << str;
	}
};


}



