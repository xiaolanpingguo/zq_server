#pragma once

#include "interface_header/base/platform.h"
#include "database_env_fwd.h"
#include "field.h"
#include <vector>
#ifdef _WIN32 // hack for broken mysql.h not including the correct winsock header for SOCKET definition, fixed in 5.7
#include <winsock2.h>
#endif
#include <mysql.h>

namespace zq {

inline uint32 sizeForType(MYSQL_FIELD* field)
{
	switch (field->type)
	{
	case MYSQL_TYPE_NULL:
		return 0;
	case MYSQL_TYPE_TINY:
		return 1;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return 2;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_FLOAT:
		return 4;
	case MYSQL_TYPE_DOUBLE:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return 8;

	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return sizeof(MYSQL_TIME);

	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return field->max_length + 1;

	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return 64;

	case MYSQL_TYPE_GEOMETRY:
		// 还有两种类型不支持MYSQL_TYPE_ENUM，MYSQL_TYPE_SET		
	default:
		LOG_WARN << "invalid field type: " << field->type;
		return 0;
	}
}

inline DatabaseFieldTypes mysqlTypeToFieldType(enum_field_types type)
{
	switch (type)
	{
	case MYSQL_TYPE_NULL:
		return DatabaseFieldTypes::Null;
	case MYSQL_TYPE_TINY:
		return DatabaseFieldTypes::Int8;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return DatabaseFieldTypes::Int16;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
		return DatabaseFieldTypes::Int32;
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return DatabaseFieldTypes::Int64;
	case MYSQL_TYPE_FLOAT:
		return DatabaseFieldTypes::Float;
	case MYSQL_TYPE_DOUBLE:
		return DatabaseFieldTypes::Double;
	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return DatabaseFieldTypes::Decimal;
	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return DatabaseFieldTypes::Date;
	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return DatabaseFieldTypes::Binary;
	default:
		LOG_WARN << "invalid field type: " << type;
		break;
	}

	return DatabaseFieldTypes::Null;
}

class ResultSet
{
public:
	ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount)
		: _rowCount(rowCount),
		_fieldCount(fieldCount),
		_result(result),
		_fields(fields)
	{
		_currentRow = new Field[_fieldCount];
	}

	~ResultSet()
	{
		cleanUp();
	}

	bool nextRow()
	{
		if (!_result)
		{
			return false;
		}

		MYSQL_ROW row = mysql_fetch_row(_result);
		if (!row)
		{
			cleanUp();
			return false;
		}

		unsigned long* lengths = mysql_fetch_lengths(_result);
		if (!lengths)
		{
			LOG_WARN << "cannot retrieve value lengths. Error: " << mysql_error(_result->handle);
			cleanUp();
			return false;
		}

		for (uint32 i = 0; i < _fieldCount; i++)
		{
			_currentRow[i].setStructuredValue(row[i], mysqlTypeToFieldType(_fields[i].type), lengths[i]);
		}

		return true;
	}

	Field const& operator[](std::size_t index) const
	{
		ASSERT(index < _fieldCount);
		return _currentRow[index];
	}

	uint64 getRowCount() const { return _rowCount; }
	uint32 getFieldCount() const { return _fieldCount; }

	Field* fetch() const { return _currentRow; }

private:

	void cleanUp()
	{
		if (_currentRow)
		{
			delete[] _currentRow;
			_currentRow = NULL;
		}

		if (_result)
		{
			mysql_free_result(_result);
			_result = NULL;
		}
	}

private:

	uint64 _rowCount;
	Field* _currentRow;
	uint32 _fieldCount;

	MYSQL_RES* _result;
	MYSQL_FIELD* _fields;

	ResultSet(ResultSet const& right) = delete;
	ResultSet& operator=(ResultSet const& right) = delete;
};

class PreparedResultSet
{
public:
	PreparedResultSet(MYSQL_STMT* stmt, MYSQL_RES* result, uint64 rowCount, uint32 fieldCount)
		:m_rowCount(rowCount),
		m_rowPosition(0),
		m_fieldCount(fieldCount),
		m_rBind(nullptr),
		m_stmt(stmt),
		m_metadataResult(result)
	{
		if (!m_metadataResult)
			return;

		if (m_stmt->bind_result_done)
		{
			delete[] m_stmt->bind->length;
			delete[] m_stmt->bind->is_null;
		}

		// 这个内存会在析构函数里删除
		m_rBind = new MYSQL_BIND[m_fieldCount];

		//- for future readers wondering where the fuck this is freed - mysql_stmt_bind_result moves pointers to these
		// from m_rBind to m_stmt->bind and it is later freed by the `if (m_stmt->bind_result_done)` block just above here
		// MYSQL_STMT lifetime is equal to connection lifetime
		// 这片内存会在MySQLPreparedStatement的析构函数里面删除
		my_bool* m_isNull = new my_bool[m_fieldCount];
		unsigned long* m_length = new unsigned long[m_fieldCount];

		memset(m_isNull, 0, sizeof(my_bool) * m_fieldCount);
		memset(m_rBind, 0, sizeof(MYSQL_BIND) * m_fieldCount);
		memset(m_length, 0, sizeof(unsigned long) * m_fieldCount);

		// 这里保存了所有的结果集
		if (mysql_stmt_store_result(m_stmt))
		{
			LOG_ERROR << "cannot bind result from MySQL server. Error: " << mysql_stmt_error(m_stmt);
			delete[] m_rBind;
			delete[] m_isNull;
			delete[] m_length;
			return;
		}

		// 获得结果的行数
		m_rowCount = mysql_stmt_num_rows(m_stmt);

		// 从元数据里面获得每个字段的相关信息
		MYSQL_FIELD* field = mysql_fetch_fields(m_metadataResult);
		std::size_t rowSize = 0;
		for (uint32 i = 0; i < m_fieldCount; ++i)
		{
			// 获得这个字段类型的长度
			// 如果是二进制或者是字符串的话，会多加一个字节
			// 因为会在接下来的代码中，这多加的这个字节会加上结束符0
			uint32 size = sizeForType(&field[i]);
			rowSize += size;

			m_rBind[i].buffer_type = field[i].type;
			m_rBind[i].buffer_length = size;
			m_rBind[i].length = &m_length[i];
			m_rBind[i].is_null = &m_isNull[i];
			m_rBind[i].error = NULL;
			m_rBind[i].is_unsigned = field[i].flags & UNSIGNED_FLAG;
		}

		// 把每一列的结果都已二进制的方式连续储存在一个数组中
		// 并且bind的结构体的指针指向这片内存的相应位置
		char* dataBuffer = new char[rowSize * m_rowCount];
		for (uint32 i = 0, offset = 0; i < m_fieldCount; ++i)
		{
			m_rBind[i].buffer = dataBuffer + offset;
			offset += m_rBind[i].buffer_length;
		}

		// 将输出结果绑定到MYSQL_BIND数据结构对象上
		if (mysql_stmt_bind_result(m_stmt, m_rBind))
		{
			LOG_ERROR << "cannot bind result from MySQL server. Error: " << mysql_stmt_error(m_stmt);
			mysql_stmt_free_result(m_stmt);
			cleanUp();
			delete[] m_isNull;
			delete[] m_length;
			return;
		}

		// 获取每行的值 
		m_rows.resize(uint32(m_rowCount) * m_fieldCount);
		while (_nextRow())
		{
			for (uint32 fIndex = 0; fIndex < m_fieldCount; ++fIndex)
			{
				unsigned long buffer_length = m_rBind[fIndex].buffer_length;
				unsigned long fetched_length = *m_rBind[fIndex].length;
				if (!*m_rBind[fIndex].is_null)
				{
					void* buffer = m_stmt->bind[fIndex].buffer;
					switch (m_rBind[fIndex].buffer_type)
					{
					case MYSQL_TYPE_TINY_BLOB:
					case MYSQL_TYPE_MEDIUM_BLOB:
					case MYSQL_TYPE_LONG_BLOB:
					case MYSQL_TYPE_BLOB:
					case MYSQL_TYPE_STRING:
					case MYSQL_TYPE_VAR_STRING:
						// warning - the string will not be null-terminated if there is no space for it in the buffer
						// when mysql_stmt_fetch returned MYSQL_DATA_TRUNCATED
						// we cannot blindly null-terminate the data either as it may be retrieved as binary blob and not specifically a string
						// in this case using Field::getCString will result in garbage
						// TODO: remove Field::getCString and use boost::string_ref (currently proposed for TS as string_view, maybe in C++17)
						if (fetched_length < buffer_length)
							*((char*)buffer + fetched_length) = '\0';
						break;
					default:
						break;
					}

					// 在这里把相关结果赋值了
					m_rows[uint32(m_rowPosition) * m_fieldCount + fIndex].setByteValue(buffer,
						mysqlTypeToFieldType(m_rBind[fIndex].buffer_type), fetched_length);

					// move buffer pointer to next part
					// 这里移动指针的意思是对于有多行记录的情况,因为在获取到每一个字段的相关信息后，
					// 需要移动一行数据的长度到下一行这个字段所在的缓冲区所在的位置
					m_stmt->bind[fIndex].buffer = (char*)buffer + rowSize;
				}
				else
				{
					m_rows[uint32(m_rowPosition) * m_fieldCount + fIndex].setByteValue(nullptr,
						mysqlTypeToFieldType(m_rBind[fIndex].buffer_type), *m_rBind[fIndex].length);
				}
			}
			m_rowPosition++;
		}
		m_rowPosition = 0;

		// 释放资源
		mysql_stmt_free_result(m_stmt);
	}

	~PreparedResultSet()
	{
		cleanUp();
	}

	Field* fetch() const
	{
		ASSERT(m_rowPosition < m_rowCount);
		return const_cast<Field*>(&m_rows[uint32(m_rowPosition) * m_fieldCount]);
	}

	Field const& operator[](std::size_t index) const
	{
		ASSERT(m_rowPosition < m_rowCount);
		ASSERT(index < m_fieldCount);
		return m_rows[uint32(m_rowPosition) * m_fieldCount + index];
	}

	bool nextRow()
	{
		// Only updates the m_rowPosition so upper level code knows in which element
		// of the rows vector to look
		if (++m_rowPosition >= m_rowCount)
			return false;

		return true;
	}

	uint64 getRowCount() const { return m_rowCount; }
	uint32 getFieldCount() const { return m_fieldCount; }

private:
	void cleanUp()
	{
		if (m_metadataResult)
		{
			mysql_free_result(m_metadataResult);
		}

		if (m_rBind)
		{
			delete[](char*)m_rBind->buffer;
			delete[] m_rBind;
			m_rBind = nullptr;
		}
	}

	bool _nextRow()
	{
		// Only called in low-level code, namely the constructor
		// Will iterate over every row of data and buffer it
		if (m_rowPosition >= m_rowCount)
			return false;

		int retval = mysql_stmt_fetch(m_stmt);
		return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
	}

private:

	std::vector<Field> m_rows;
	uint64 m_rowCount;
	uint64 m_rowPosition;
	uint32 m_fieldCount;

	MYSQL_BIND * m_rBind;
	MYSQL_STMT* m_stmt;
	MYSQL_RES* m_metadataResult;    ///< Field metadata, returned by mysql_stmt_result_metadata

	PreparedResultSet(PreparedResultSet const& right) = delete;
	PreparedResultSet& operator=(PreparedResultSet const& right) = delete;
};
}



