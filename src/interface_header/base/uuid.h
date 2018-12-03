#pragma once

#include "platform.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace zq{

struct uuid
{
	constexpr uuid(uint64 id = 0) : data_(id){}
	uuid(const uuid& that)
	{
		if (this != &that)
		{
			this->data_ = that.data_;
		}
	}
	uuid& operator=(const uuid& that)
	{
		if (this != &that)
		{
			this->data_ = that.data_;
		}

		return *this;
	}

	operator uint64()
	{
		return data_;
	}

    bool operator == (const uuid& id) const
    {
        return this->data_ == id.data_;
    }

    bool operator != (const uuid& id) const
    {
        return this->data_ != id.data_;
    }

    bool operator < (const uuid& id) const
    {
        return this->data_ < id.data_;
    }

	int64 data_;
};

}
