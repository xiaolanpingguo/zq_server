#pragma once

#include "interface_header/platform.h"
#include <stack>
#include <vector>
#include <map>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>


namespace zq {


enum class RAPIDJSON_TYPE
{
	EN_BOOL,
	EN_INT32,
	EN_UINT32,
	EN_INT64,
	EN_UINT64,
	EN_ARRAY,
};

class RapidJsonReader : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, RapidJsonReader>
{
public:
	bool readMessage(std::string const& json)
	{
		if (json.empty())
		{
			return false;
		}

		rapidjson::StringStream ss(json.c_str());
		rapidjson::ParseResult result = _reader.Parse(ss, *this);

		ASSERT(result.IsError() || (_state.empty()));

		return !result.IsError() && _errors.empty();
	}

	bool Key(const Ch* str, rapidjson::SizeType length, bool copy)
	{
		_state.push(str);
		return true;
	}

	bool Null()
	{
		_state.pop();
		return true;
	}

	bool Bool(bool b)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Bool, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(b));
		return true;
	}
	bool Int(int32 i)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Int, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(i));
		return true;
	}
	bool Uint(uint32 i)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Uint, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(i));
		return true;
	}
	bool Int64(int64 i)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Int64, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(i));
		return true;
	}
	bool Uint64(uint64 i)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Uint64, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(i));
		return true;
	}
	bool Double(double d)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::Double, _state is empty.");
			return false;
		}

		setField(_state.top(), std::to_string(d));
		return true;
	}
	bool String(const Ch* str, rapidjson::SizeType length, bool copy)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::String, _state is empty.");
			return false;
		}

		setField(_state.top(), str);
		return true;
	}
	bool StartObject()
	{
		return true;
	}
	bool EndObject(rapidjson::SizeType memberCount)
	{
		if (!_state.empty())
			_state.pop();

		return true;
	}
	bool StartArray()
	{
		return true;
	}
	bool EndArray(rapidjson::SizeType memberCount)
	{
		if (_state.empty())
		{
			_errors.emplace_back("RapidJsonReader::EndArray, _state is empty.");
			return false;
		}

		_state.pop();
		return true;
	}

	std::vector<std::string> const& GetErrors() const { return _errors; }

private:
	void setField(const std::string& key, const std::string& value)
	{
		params_.insert(std::make_pair(key, value));
		_state.pop();
	}

private:

	rapidjson::Reader _reader;
	std::stack<std::string> _state;
	std::vector<std::string> _errors;
	std::map<std::string, std::string> params_;
};


class RapidJsonWrite
{
public:
	RapidJsonWrite() 
		: strBuf_(),
		writer_(strBuf_)
	{
	}

	~RapidJsonWrite() { writer_.Reset(strBuf_); }

	void startObject() { writer_.StartObject(); }
	void endObject() { writer_.EndObject(); }

	void startArray() { writer_.StartArray(); }
	void endArray() { writer_.EndArray(); }

	void key(const char* key) { writer_.Key(key); }

	const char* getString() { return strBuf_.GetString(); }

	template<typename T> void write(const char* key, const T& value)
	{
		writer_.Key(key);
		writer_.Null();
	}

	template<> void write<std::string>(const char* key, const std::string& value)
	{
		writer_.Key(key);
		writer_.String(value.c_str());
	}

	template<> void write<const char*>(const char* key, const char* const& value)
	{
		writer_.Key(key);
		writer_.String(value);
	}

	template<> void write<bool>(const char* key, bool const& value)
	{
		writer_.Key(key);
		writer_.Bool(value);
	}

	template<> void write<int16>(const char* key, int16 const& value)
	{
		writer_.Key(key);
		writer_.Int(value);
	}

	template<> void write<uint16>(const char* key, uint16 const& value)
	{
		writer_.Key(key);
		writer_.Uint(value);
	}

	template<> void write<int32>(const char* key, int32 const& value)
	{
		writer_.Key(key);
		writer_.Int(value);
	}

	template<> void write<uint32>(const char* key, uint32 const& value)
	{
		writer_.Key(key);
		writer_.Uint(value);
	}

	template<> void write<int64>(const char* key, int64 const& value)
	{
		writer_.Key(key);
		writer_.Int64(value);
	}

	template<> void write<uint64>(const char* key, uint64 const& value)
	{
		writer_.Key(key);
		writer_.Uint64(value);
	}

	template<> void write<float>(const char* key, float const& value)
	{
		writer_.Key(key);
		writer_.Double(value);
	}

	template<> void write<double>(const char* key, double const& value)
	{
		writer_.Key(key);
		writer_.Double(value);
	}

	template<> void write<std::vector<int32>>(const char* key, std::vector<int32> const& value)
	{
		writer_.Key(key);
		writer_.StartArray();
		for (size_t i = 0; i < value.size(); ++i)
		{
			writer_.Int(value[i]);
		}
		writer_.EndArray();
	}

	template<> void write<std::vector<int64>>(const char* key, std::vector<int64> const& value)
	{
		writer_.Key(key);
		writer_.StartArray();
		for (size_t i = 0; i < value.size(); ++i)
		{
			writer_.Int64(value[i]);
		}
		writer_.EndArray();
	}

	template<> void write<std::vector<std::string>>(const char* key, std::vector<std::string> const& value)
	{
		writer_.Key(key);
		writer_.StartArray();
		for (size_t i = 0; i < value.size(); ++i)
		{
			writer_.String(value[i].c_str());
		}
		writer_.EndArray();
	}

private:
	rapidjson::StringBuffer strBuf_;
	rapidjson::Writer<rapidjson::StringBuffer> writer_;
};
}