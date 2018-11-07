#pragma once

#include <map>
#include <string>
#include <list>
#include <functional> 
#include <sstream>
#include "baselib/interface_header/platform.h"
#include "crc32.hpp"

namespace zq {



// 抽象一个虚拟节点
class IVirtualNode
{
public:

	IVirtualNode(const int vir_id) :virtualIndex_(vir_id){}
	IVirtualNode() : virtualIndex_(0){}

	virtual ~IVirtualNode()
	{
		virtualIndex_ = 0;
	}

	virtual std::string getDataStr() const
	{
		return "";
	}

	std::string toStr() const
	{
		std::ostringstream strInfo;
		strInfo << getDataStr() << "-" << virtualIndex_;
		return strInfo.str();
	}

private:
	int virtualIndex_;
};

template <typename T>
class VirtualNode : public IVirtualNode
{
public:
	VirtualNode(const T tData, const int nVirID) : IVirtualNode(nVirID)
	{
		data_ = tData;
	}
	VirtualNode()
	{

	}
	virtual ~VirtualNode()
	{
	}

	virtual std::string getDataStr() const
	{
		return lexical_cast<std::string>(data_);
	}

	T data_;
};

class IHasher
{
public:
	virtual ~IHasher(){}
	virtual uint32 getHashValue(const IVirtualNode& vNode) = 0;
};

class Hasher : public IHasher
{
public:
	virtual ~Hasher() {}
	virtual uint32 getHashValue(const IVirtualNode& vNode)
	{
		std::string vnode = vNode.toStr();
		return zq::crc32(vnode);
	}
};

template <typename T>
class IConsistentHash
{
public:
	virtual~IConsistentHash() {};
	virtual std::size_t size() const = 0;
	virtual bool empty() const = 0;

	virtual void clearAll() = 0;
	virtual void insert(const T& name) = 0;
	virtual void insert(const VirtualNode<T>& xNode) = 0;

	virtual bool exist(const VirtualNode<T>& xInNode) = 0;
	virtual void erase(const T& name) = 0;
	virtual std::size_t erase(const VirtualNode<T>& xNode) = 0;

	virtual bool getSuitNodeRandom(VirtualNode<T>& node) = 0;
	virtual bool getSuitNodeConsistent(VirtualNode<T>& node) = 0;
	virtual bool getSuitNode(const T& name, VirtualNode<T>& node) = 0;
	virtual bool getSuitNode(uint32 hashValue, VirtualNode<T>& node) = 0;

	virtual bool getNodeList(std::list<VirtualNode<T>>& nodeList) = 0;
};

template <typename T>
class ConsistentHash : public IConsistentHash<T>
{
public:
	ConsistentHash()
	{
		hasher_ = new Hasher();
	}

	virtual ~ConsistentHash()
	{
		delete hasher_;
		hasher_ = nullptr;
	}

public:
	virtual std::size_t size() const
	{
		return nodes_.size();
	}

	virtual bool empty() const
	{
		return nodes_.empty();
	}

	virtual void clearAll()
	{
		nodes_.clear();
	}

	virtual void insert(const T& name)
	{
		for (int i = 0; i < NODE_CONUNT; ++i)
		{
			VirtualNode<T> vNode(name, i);
			insert(vNode);
		}
	}

	virtual void insert(const VirtualNode<T>& xNode)
	{
		uint32 hash = hasher_->getHashValue(xNode);
		auto it = nodes_.find(hash);
		if (it == nodes_.end())
		{
			nodes_.insert(std::make_pair(hash, xNode));
		}
	}

	virtual bool exist(const VirtualNode<T>& xInNode)
	{
		uint32 hash = hasher_->getHashValue(xInNode);
		auto it = nodes_.find(hash);
		if (it != nodes_.end())
		{
			return true;
		}

		return false;
	}

	virtual void erase(const T& name)
	{
		for (int i = 0; i < NODE_CONUNT; ++i)
		{
			VirtualNode<T> vNode(name, i);
			erase(vNode);
		}
	}

	virtual std::size_t erase(const VirtualNode<T>& xNode)
	{
		uint32 hash = hasher_->getHashValue(xNode);
		return nodes_.erase(hash);
	}

	// 随机获得一个节点
	virtual bool getSuitNodeRandom(VirtualNode<T>& node)
	{
		int nID = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		return getSuitNode(nID, node);
	}

	// 默认是获取hash值最小的节点
	virtual bool getSuitNodeConsistent(VirtualNode<T>& node)
	{
		return getSuitNode(0, node);
	}

	// 获得一个节点
	virtual bool getSuitNode(const T& name, VirtualNode<T>& node)
	{
		std::string str = lexical_cast<std::string>(name);
		uint32 nCRC32 = zq::crc32(str);
		return getSuitNode(nCRC32, node);
	}

	virtual bool getSuitNode(uint32 hashValue, VirtualNode<T>& node)
	{
		if (nodes_.empty())
		{
			return false;
		}

		auto it = nodes_.lower_bound(hashValue);
		if (it == nodes_.end())
		{
			it = nodes_.begin();
		}

		node = it->second;

		return true;
	}

	virtual bool getNodeList(std::list<VirtualNode<T>>& nodeList)
	{
		for (auto it = nodes_.begin(); it != nodes_.end(); ++it)
		{
			nodeList.push_back(it->second);
		}

		return true;
	}

private:

	// 虚拟节点的数量，节点数量越多，数据分布越均匀
	static const int NODE_CONUNT = 500;

	std::map<uint32, VirtualNode<T>> nodes_;
	IHasher* hasher_;
};


}