#pragma once


#include "interface_header/base/platform.h"
#include <vector>

namespace zq {

class MessageBuffer
{
	typedef std::vector<uint8>::size_type size_type;

public:
	MessageBuffer() : _wpos(0), _rpos(0), _storage()
	{
		_storage.resize(4096);
	}

	explicit MessageBuffer(std::size_t initialSize) : _wpos(0), _rpos(0), _storage()
	{
		_storage.resize(initialSize);
	}

	MessageBuffer(MessageBuffer const& right) : _wpos(right._wpos), _rpos(right._rpos), _storage(right._storage)
	{
	}

	MessageBuffer(MessageBuffer&& right) : _wpos(right._wpos), _rpos(right._rpos), _storage(right.move()) { }

	void reset()
	{
		_wpos = 0;
		_rpos = 0;
	}

	void resize(size_type bytes)
	{
		_storage.resize(bytes);
	}

	uint8* getBasePointer() { return _storage.data(); }

	uint8* getReadPointer() { return getBasePointer() + _rpos; }

	uint8* getWritePointer() { return getBasePointer() + _wpos; }

	void readCompleted(size_type bytes) { _rpos += bytes; }

	void writeCompleted(size_type bytes) { _wpos += bytes; }

	size_type getActiveSize() const { return _wpos - _rpos; }

	size_type getRemainingSpace() const { return _storage.size() - _wpos; }

	size_type getBufferSize() const { return _storage.size(); }

	void normalize()
	{
		if (_rpos)
		{
			if (_rpos != _wpos)
			{
				memmove(getBasePointer(), getReadPointer(), getActiveSize());
			}

			_wpos -= _rpos;
			_rpos = 0;
		}
	}

	// 确保有足够的空间，调用这个之前必须先调用normalize
	void ensureFreeSpace()
	{
		if (getRemainingSpace() == 0)
		{
			_storage.resize(_storage.size() * 3 / 2);
		}
	}

	void write(void const* data, std::size_t size)
	{
		if (size)
		{
			memcpy(getWritePointer(), data, size);
			writeCompleted(size);
		}
	}

	std::vector<uint8>&& move()
	{
		_wpos = 0;
		_rpos = 0;
		return std::move(_storage);
	}

	MessageBuffer& operator=(MessageBuffer const& right)
	{
		if (this != &right)
		{
			_wpos = right._wpos;
			_rpos = right._rpos;
			_storage = right._storage;
		}

		return *this;
	}

	MessageBuffer& operator=(MessageBuffer&& right)
	{
		if (this != &right)
		{
			_wpos = right._wpos;
			_rpos = right._rpos;
			_storage = right.move();
		}

		return *this;
	}

private:
	size_type _wpos;
	size_type _rpos;
	std::vector<uint8> _storage;
};

}


