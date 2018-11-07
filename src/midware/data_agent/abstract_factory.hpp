#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <type_traits>

namespace zq {


template<class Type>
inline std::string CreateFullDataKey()
{
	std::string szKey;
	szKey = typeid<Type>.name();
	return szKey;
}

template<class Type>
inline std::string CreateFullDataKey(const std::string& szKey)
{
	std::string szFullKey;
	szFullKey = typeid<Type>.name();
	szFullKey = szFullKey + ":";
	szFullKey = szFullKey + szKey;
	return szFullKey;
}

template<class Type>
inline std::string CreateFullDataKey(const std::string& szKey1, const std::string& szKey2)
{
	std::string szFullKey;
	szFullKey = typeid<Type>.name();
	szFullKey = szFullKey + ":";
	szFullKey = szFullKey + szKey1;
	szFullKey = szFullKey + ":";
	szFullKey = szFullKey + szKey2;
	return szFullKey;
}

class AbstractProduct
{
public:
	virtual ~AbstractProduct() {}
	virtual void setKey(const long long nKey){ m_nKey = nKey; }
	const long long getKey() const { return m_nKey; }
	virtual void SetRedisKey(const std::string& key)
	{
		m_szRedisKey = key;
	}

	virtual std::string GetRedisKey() { return m_szRedisKey; }
	
	size_t GetIndex() const{ return m_nIndex; }
	virtual void SetMemDataFlag(bool bFlag){ m_bMemData = bFlag; }
	virtual bool IsMemData(){ return m_bMemData; }

protected:
	long long m_nKey;
	std::string m_szRedisKey;
	size_t m_nIndex;
	bool m_bMemData;
	char m_bySaveType = 0;					// Ĭ���ǲ����κα���
};

class ProductMem
{
public:
	virtual ~ProductMem(){}
	virtual AbstractProduct* Constructor() = 0;
};


template<template<typename> class T>
class AbstractFactory
{
	// @note
	//     - ԭ����ֱ���õ�hash_code������cppreference��˵��the same value may be returned
	//   for different types. The value can also change between invocations of the same
	//   program�������ǲ�����д��TypeInfoRef
	//     - �Ժ�Ӧ���� type_index ������
	using TypeInfoRef = std::reference_wrapper<const std::type_info>;
	struct Hasher
	{
		std::size_t operator()(TypeInfoRef code) const
		{
			return code.get().hash_code();
		}
	};
	struct EqualTo
	{
		bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const
		{
			return lhs.get() == rhs.get();
		}
	};

public:
	using ProductListType = std::map<long long, std::shared_ptr<AbstractProduct>>;
	using ProductsType = std::unordered_map<TypeInfoRef, ProductListType, Hasher, EqualTo>;

public:
	virtual ~AbstractFactory() {}

	// @note ����ÿһ�����Ͳ�Ʒ�������������CServerUserItemһ�������ÿ�ֲ�Ʒ��һ����
	//   ����Ĭ�Ͽ��Բ���keyֵ�����ﷵ��keyҲ���ã��ܶ�ط���Ҫ���ش����Ķ���
	template<class U, typename... Args>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> createData(Args&&... args)
	{
		try
		{
			using DataType = T<typename std::remove_cv<U>::type>;
			auto pData = std::make_shared<DataType>();
			std::string szKey = CreateFullDataKey<U>();
			pData.get()->setKey(0);
			pData.get()->SetRedisKey(szKey);
			pData->SetMemDataFlag(false);
			return pData;
		}
		catch(...)
		{
			return nullptr;
		}
	}

	template<class U, typename... Args>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> createDataByKey(std::string szKey, Args&&... args)
	{
		try
		{
			if (szKey == "")
			{
				ASSERT(FALSE);
				return nullptr;
			}

			auto pData = AllocMem<U>();
			pData.get()->SetRedisKey(CreateFullDataKey<U>(szKey));
			pData->SetMemDataFlag(false);
			return pData;
		}
		catch (...)
		{
			return nullptr;
		}
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getDataNoRedis(std::string szKeys)
	{
		using DataType = T<typename std::remove_cv<U>::type>;
		std::string szKey = "";

		if (szKeys == "")
		{
			szKey = typeid<U>().name();
		}
		else
		{
			szKey = CreateFullDataKey<U>(szKeys);
		}

		auto it = DataBufferUpdater::get_mutable_instance().FindRedisData(szKey);
		if (it == nullptr)
			return nullptr

		std::shared_ptr<DataType> ptr = boost::dynamic_pointer_cast<DataType>(it);
		return ptr;
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getData(std::string szKeys)
	{
		using DataType = T<typename std::remove_cv<U>::type>;
		std::string szKey = "";

		if (szKeys == "")
		{
			szKey = typeid<U>().name();
		}
		else
		{
			szKey = CreateFullDataKey<U>(szKeys);
		}

		auto it = DataBufferUpdater::get_mutable_instance().FindRedisData(szKey);
		if (it == nullptr)
			return ReadDataFromRedis<typename std::remove_cv<U>::type>(szKeys);

		std::shared_ptr<DataType> ptr = boost::dynamic_pointer_cast<DataType>(it);
		return ptr;
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getDataCreateNoRedis(std::string szKeys)
	{
		auto rp = getData<typename std::remove_cv<U>::type>(szKeys);
		if (rp == nullptr)
		{
			if (szKeys == "")
			{
				rp = createData<typename std::remove_cv<U>::type>();
			}
			else
			{
				rp = createDataByKey<typename std::remove_cv<U>::type>(szKeys);
			}

			rp->OnCreateData(szKeys);
		}
		return rp;
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getDataCreate(std::string szKeys)
	{
		auto rp = getData<typename std::remove_cv<U>::type>(szKeys);
		if (rp == nullptr)
		{
			if (szKeys == "")
			{
				rp = createData<typename std::remove_cv<U>::type>();
			}
			else
			{
				rp = createDataByKey<typename std::remove_cv<U>::type>(szKeys);
			}

			rp->OnCreateData(szKeys);
		}
		return rp;
	}
	

	// @note ʹ������ӿ��п��ܻ�ɾ�����ݵĸ���㣨���ɾ���������һ�����ݣ������Ե��ô�
	//    Ҫ����ʹ�õ�����
	template<class U>
	bool deleteData(long long nKey = 0)
	{
		auto pDataPtr = getData<typename std::remove_cv<U>::type>(nKey);
		if (pDataPtr != nullptr)
			DataBufferUpdater::get_mutable_instance().delToUpdateList(pDataPtr);
		return true;
	}

	template<class U>
	bool deleteData(std::string &szKey)
	{
		auto pDataPtr = getData<typename std::remove_cv<U>::type>(szKey);
		if (pDataPtr != nullptr)
			DataBufferUpdater::get_mutable_instance().delToUpdateList(pDataPtr);
		return true;
	}

	void clearData()
	{
		for(auto& ref : m_mapProducts)
		{
			ref.second.clear();
		}
		m_mapProducts.clear();
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> ReadDataFromRedis(std::string szKeys)
	{
		char buf[16384];
		typename std::remove_cv<U>::type data;
		std::string szKey = "";
		if (szKeys != "")
			szKey = CreateFullDataKey<U>(szKeys);
		else
			szKey = CreateFullDataKey<U>();
		int nSize = 16384;
		char* pBuff = RedisMgr::get_mutable_instance().GetImpl()->GetValue(szKey.c_str(), buf, nSize);
		if (pBuff == nullptr)
			return nullptr;

		// 		if (boost::serialization::is_bitwise_serializable<U>())
		// 		{
		UnserializeFromBinary<typename std::remove_cv<U>::type>(std::string(pBuff, nSize), data);
		// 		}
		// 		else
		// 		{
		// 			memcpy((char*)&data, buf, nSize);
		// 		}
		std::shared_ptr<T<typename std::remove_cv<U>::type>> newData = nullptr;
		if (szKeys == "")
		{
			newData = createData<U>();
		}
		else
		{
			newData = createDataByKey<U>(szKeys);
		}
		newData->SetRedisKey(szKey);
		newData->getDataRef(0) = data;
		return newData;
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getMemDataCreate(int nKey = 0)
	{
		auto rp = getMemData<typename std::remove_cv<U>::type>(nKey);
		if (rp == nullptr)
		{
			rp = createMemDataByKey<typename std::remove_cv<U>::type>(nKey);
		}

		return rp;
	}

	template<class U>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> getMemData(int nKey)
	{
		using DataType = T<typename std::remove_cv<U>::type>;
		auto iter = m_mapProducts.find(typeid(DataType));
		if (iter != m_mapProducts.end())
		{
			const ProductListType& productList = iter->second;
			auto itProduct = productList.find(nKey);
			if (itProduct != productList.end())
			{
				return std::static_pointer_cast<DataType>(itProduct->second);
			}
		}
		return nullptr;
	}

	// ��Ϊ���˱��ģ�壬�����޷���Ĭ�ϲ����޷�����
	template<class U, typename... Args>
	std::shared_ptr<T<typename std::remove_cv<U>::type>> createMemDataByKey(int nKey, Args&&... args)
	{
		using DataType = T<typename std::remove_cv<U>::type>;
		const std::type_info& typeID = typeid(DataType);

		try
		{
			deleteMemData<typename std::remove_cv<U>::type>(nKey);
			auto pData = std::shared_ptr<DataType>(new DataType(boost::forward<Args>(args)...));
			pData->setKey(nKey);
			pData->SetMemDataFlag(true);

			auto pair = std::make_pair(pData->getKey(), std::static_pointer_cast<AbstractProduct>(pData));
			auto iter = m_mapProducts.find(typeID);
			if (iter != m_mapProducts.end())
			{
				iter->second.insert(pair);
			}
			else
			{
				ProductListType mapProduct{ pair };
				m_mapProducts[typeID] = mapProduct;
			}
			return pData;
		}
		catch (...)
		{
			return nullptr;
		}
	}

	// @note ʹ������ӿ��п��ܻ�ɾ�����ݵĸ���㣨���ɾ���������һ�����ݣ������Ե��ô�
	//    Ҫ����ʹ�õ�����
	template<class U>
	bool deleteMemData(int nKey = 0)
	{
		using DataType = T<typename std::remove_cv<U>::type>;
		auto iter = m_mapProducts.find(typeid(DataType));
		if (iter != m_mapProducts.end())
		{
			ProductListType& productList = iter->second;
			//auto itProduct = productList.begin();
			auto itProduct = (nKey != 0) ? productList.find(nKey) : productList.begin();
			if (itProduct != productList.end())
			{
				itProduct->second.reset();
				productList.erase(itProduct);
			}
			else
			{
				return false;
			}
			if (iter->second.size() == 0)
			{
				m_mapProducts.erase(iter);
			}
			return true;
		}
		return false;
	}


protected:
	ProductsType m_mapProducts;
	std::map<size_t, std::shared_ptr<ProductMem>> _mapMem; // ����ؽṹ
};

}
// @note map��for_each������ĳЩʱ��Ҫ�� map::reference

// �÷�
// createData<ST_DataType>();
// std::shared_ptr<TDataType<ST_DataType>> pData = getData<ST_DataType>();