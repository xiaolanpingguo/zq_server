#pragma once

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <typeinfo>
#include <memory>
#include "consistent_hash.hpp"
#include "baselib/interface_header/platform.h"

namespace zq {

template <typename T, typename TD>
class MapEx
{
public:
	using MapObjects = std::map<T, std::shared_ptr<TD>>;

	MapEx()
	{
	};
	virtual ~MapEx()
	{
	};

	virtual bool existElement(const T& name)
	{
		auto it = mapObjects_.find(name);
		if (it != mapObjects_.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool addElement(const T& name, const std::shared_ptr<TD> data)
	{
		if (data == nullptr)
		{
			return false;
		}

		auto it = mapObjects_.find(name);
		if (it == mapObjects_.end())
		{
			mapObjects_.insert(std::make_pair(name, data));

			return true;
		}

		return false;
	}

	virtual bool removeElement(const T& name)
	{
		auto it = mapObjects_.find(name);
		if (it != mapObjects_.end())
		{
			mapObjects_.erase(it);

			return true;
		}

		return false;
	}

	virtual std::shared_ptr<TD> getElement(const T& name)
	{
		auto it = mapObjects_.find(name);
		if (it != mapObjects_.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	virtual std::shared_ptr<TD> first()
	{
		if (mapObjects_.size() <= 0)
		{
			return nullptr;
		}

		objectCurIter_ = mapObjects_.begin();
		if (objectCurIter_ != mapObjects_.end())
		{
			return objectCurIter_->second;
		}
		else
		{
			return nullptr;
		}
	}

	virtual std::shared_ptr<TD> next()
	{
		if (objectCurIter_ == mapObjects_.end())
		{
			return nullptr;
		}

		++objectCurIter_;
		if (objectCurIter_ != mapObjects_.end())
		{
			return objectCurIter_->second;
		}
		else
		{
			return nullptr;
		}
	}

	virtual std::shared_ptr<TD> first(T& name)
	{
		if (mapObjects_.size() <= 0)
		{
			return nullptr;
		}

		objectCurIter_ = mapObjects_.begin();
		if (objectCurIter_ != mapObjects_.end())
		{
			name = objectCurIter_->first;
			return objectCurIter_->second;
		}
		else
		{
			return nullptr;
		}
	}

	virtual std::shared_ptr<TD> next(T& name)
	{
		if (objectCurIter_ == mapObjects_.end())
		{
			return nullptr;
		}

		objectCurIter_++;
		if (objectCurIter_ != mapObjects_.end())
		{
			name = objectCurIter_->first;
			return objectCurIter_->second;
		}
		else
		{
			return nullptr;
		}
	}

	virtual bool clear()
	{
		mapObjects_.clear();
		return true;
	}

	int size()
	{
		return (int)mapObjects_.size();
	}


protected:
	MapObjects mapObjects_;
	typename MapObjects::iterator objectCurIter_;
};


template <typename T, typename TD>
class ConsistentHashMap : public MapEx<T, TD>
{
public:
	virtual std::shared_ptr<TD> getElementBySuitRandom()
	{
		VirtualNode<T> vNode;
		if (consistentHash_.getSuitNodeRandom(vNode))
		{
			auto it = MapEx<T, TD>::mapObjects_.find(vNode.data_);
			if (it != MapEx<T, TD>::mapObjects_.end())
			{
				return it->second;
			}
		}

		return nullptr;
	}

	virtual std::shared_ptr<TD> getElementBySuitConsistent()
	{
		VirtualNode<T> vNode;
		if (consistentHash_.getSuitNodeConsistent(vNode))
		{
			auto it = MapEx<T, TD>::mapObjects_.find(vNode.data_);
			if (it != MapEx<T, TD>::mapObjects_.end())
			{
				return it->second;
			}
		}

		return nullptr;
	}

	std::shared_ptr<TD> getElementBySuit(const std::string& key)
	{
		VirtualNode<T> vNode;
		if (consistentHash_.getSuitNode(key, vNode))
		{
			auto it = MapEx<T, TD>::mapObjects_.find(vNode.data_);
			if (it != MapEx<T, TD>::mapObjects_.end())
			{
				return it->second;
			}
		}

		return nullptr;
	}

	virtual bool addElement(const T& name, const std::shared_ptr<TD> data)
	{
		if (data == nullptr)
		{
			return false;
		}

		auto it = MapEx<T, TD>::mapObjects_.find(name);
		if (it == MapEx<T, TD>::mapObjects_.end())
		{
			MapEx<T, TD>::mapObjects_.insert(std::make_pair(name, data));
			consistentHash_.insert(name);

			return true;
		}

		return false;
	}

	virtual bool removeElement(const T& name) override
	{
		auto it = MapEx<T, TD>::mapObjects_.find(name);
		if (it != MapEx<T, TD>::mapObjects_.end())
		{
			MapEx<T, TD>::mapObjects_.erase(it);
			consistentHash_.erase(name);

			return true;
		}

		return false;
	}

	virtual bool clear() override
	{
		MapEx<T, TD>::mapObjects_.clear();
		consistentHash_.clearAll();
		return true;
	}

private:
	ConsistentHash<T> consistentHash_;
};


}