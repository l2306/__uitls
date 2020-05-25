
#ifndef UTILS_HEAP_H
#define UTILS_HEAP_H

#include <vector>
#include <map>
#include <cstddef>

#include <iostream>
#include <string>

using namespace std;
/*
class HashFunc
{
public:
	int operator()(const string & key)
	{
		int hash = 0;		//此哈希取值函数 不是很好
		for (int i = 0; i < key.length(); ++i)
			hash = hash << 7 ^ key[i];
		return (hash & 0x7FFFFFFF);
	}
};
class EqualKey
{
public:
	bool operator()(const string & A, const string & B)
	{
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};
*/

//====
//

template <typename DataT, typename KeyT, typename CmpT>
class HeapT2
{
public:
	HeapT2(int max=-1);
	virtual ~HeapT2();

	int size() const;
	bool empty() const;

	void clear();
	void reset();

	bool insert(const DataT& data);
	bool update(const DataT& data);
	bool remove(const DataT& data);
	bool replace(const DataT& data_old,const DataT& data_new);

	DataT top(bool * pbOK = NULL) const;

protected:
	CmpT                m_trait;
	size_t              m_nSizeCur;
	size_t              m_nSizeMax;
	vector<DataT>       m_vecData;          //
	map<KeyT, size_t>   m_mapIdx2Pos;       //键值 与 下标   最好改为 hashmap
	//HashMap<KeyT, size_t, HashFunc, EqualKey> hashmap(1024, -1);
	void moveData_up(size_t idx);
	void moveData_dn(size_t idx);
	void _swap(size_t idx1, size_t idx2);
};
//cmpVal_FZ 父子比较 

template <typename DataT, typename KeyT, typename CmpT>
HeapT2 <DataT, KeyT, CmpT>::HeapT2(int max)
	: m_nSizeCur(0),m_nSizeMax(max)
{
}

template <typename DataT, typename KeyT, typename CmpT>
HeapT2 <DataT, KeyT, CmpT>::~HeapT2()
{
}

template <typename DataT, typename KeyT, typename CmpT>
int HeapT2 <DataT, KeyT, CmpT>::size() const
{
	return m_nSizeCur ;
}

template <typename DataT, typename KeyT, typename CmpT>
bool HeapT2 <DataT, KeyT, CmpT>::empty() const
{
	return m_nSizeCur == 0;
}

template <typename DataT, typename KeyT, typename CmpT>
void HeapT2 <DataT, KeyT, CmpT>::clear()
{
	m_vecData.clear();
	m_mapIdx2Pos.clear();
	m_nSizeCur = 0;
}

template <typename DataT, typename KeyT, typename CmpT>
void HeapT2 <DataT, KeyT, CmpT>::reset()
{
	m_mapIdx2Pos.clear();
	m_nSizeCur = 0;
}

template <typename DataT, typename KeyT, typename CmpT>
bool HeapT2 <DataT, KeyT, CmpT>::insert(const DataT& data)
{
	if (m_nSizeCur==m_nSizeMax)
		return false;
	// 确保存储空间 
	if (m_nSizeCur == m_vecData.size())
		m_vecData.push_back(data);

	++m_nSizeCur;

	// 设置数据 
	m_vecData[m_nSizeCur - 1] = data;
	m_mapIdx2Pos[getKey(data)] = m_nSizeCur - 1;

	// 向上追溯 
	moveData_up(m_nSizeCur - 1);
	return true;
}

template <typename DataT, typename KeyT, typename CmpT>
bool HeapT2 <DataT, KeyT, CmpT>::update(const DataT& data)
{
//C++标准的规定
//   引用模板类型内部的type必须显示告诉编译器这是个type(iterator)而不是variable.
//   默认情况下认为模板内部的成员为变量
// typename map<KeyT, size_t>::iterator iter;
	KeyT key = getKey(data);
	typename map<KeyT, size_t>::iterator iter = m_mapIdx2Pos.find(key);
	if (m_mapIdx2Pos.end() == iter)
		return false;

	size_t idx = iter->second;
	if (idx >= m_nSizeCur)
		return false;

	m_vecData[idx] = data;

    //1，比父节点小，那么，需要采用向上调整 
    //2，比子节点中的某个小，那么，需要采用向下调整 
//    if ( idx > 0 && !(cmpVal(m_vecData[(idx-1)/2], m_vecData[idx])) { 
		moveData_up(idx);
//    else 
        moveData_dn(idx);

	return true;
}

template <typename DataT, typename KeyT, typename CmpT>
bool HeapT2 <DataT, KeyT, CmpT>::remove(const DataT& data)
{
	KeyT key = getKey(data);
	typename map<KeyT, size_t>::iterator iter = m_mapIdx2Pos.find(key);
	if (m_mapIdx2Pos.end() == iter)
		return false;

	size_t idx = iter->second;
	if (idx >= m_nSizeCur)
		return false;

	m_mapIdx2Pos.erase(iter);
	swap(m_vecData[idx], m_vecData[m_nSizeCur - 1]);
	m_mapIdx2Pos[getKey(m_vecData[idx])] = idx;				//最后一个值的地址变了 
	m_vecData.pop_back();
	--m_nSizeCur;

	moveData_up(idx);
	moveData_dn(idx);

	return true;
}
template <typename DataT, typename KeyT, typename CmpT>
bool HeapT2 <DataT, KeyT, CmpT>::replace(const DataT& data_old,const DataT& data_new)
{
	KeyT key = getKey(data_old);
	typename map<KeyT, size_t>::iterator iter = m_mapIdx2Pos.find(key);
	if (m_mapIdx2Pos.end() == iter)
		return false;

	size_t idx = iter->second;
	if (idx >= m_nSizeCur)
		return false;

	m_mapIdx2Pos.erase(iter);
	m_vecData[idx]=data_new;
	m_mapIdx2Pos[getKey(data_new)] = idx;

	moveData_up(idx);
	moveData_dn(idx);
	
	return true;
}

template <typename DataT, typename KeyT, typename CmpT>
DataT HeapT2 <DataT, KeyT, CmpT>::top(bool * pbOK) const
{
	if (!m_vecData.empty())
	{
		if (NULL != pbOK)
			*pbOK = true;
		return m_vecData[0];
	}
	if (NULL != pbOK)
	{
		*pbOK = false;
	}
	return DataT();
}

template <typename DataT, typename KeyT, typename CmpT>
void HeapT2 <DataT, KeyT, CmpT>::moveData_up(size_t idx)
{
	if (idx > 0)
	{
		size_t parentIdx = (idx - 1) / 2;
		bool is = m_trait.cmpVal_FZ(m_vecData[parentIdx], m_vecData[idx]);
		if (!is)
		{
			_swap(idx, parentIdx);
			moveData_up(parentIdx);
		}	
	}
}

template <typename DataT, typename KeyT, typename CmpT>
void HeapT2 <DataT, KeyT, CmpT>::moveData_dn(size_t idx)
{
	size_t idxChild_L = idx * 2 + 1;
	size_t idxChild_R = idx * 2 + 2;

	bool bLeft = (idxChild_L < m_nSizeCur) && !(m_trait.cmpVal_FZ(m_vecData[idx], m_vecData[idxChild_L]));
	if (bLeft)
	{
		_swap(idx, idxChild_L);
		moveData_dn(idxChild_L);
	}
	bool bRight = (idxChild_R < m_nSizeCur) && !(m_trait.cmpVal_FZ(m_vecData[idx], m_vecData[idxChild_R]));
	if (bRight)
	{
		_swap(idx, idxChild_R);
		moveData_dn(idxChild_R);
	}
}

template <typename DataT, typename KeyT, typename CmpT>
void HeapT2 <DataT, KeyT, CmpT>::_swap(size_t idx1, size_t idx2)
{
	swap(m_vecData[idx1], m_vecData[idx2]);
	m_mapIdx2Pos[getKey(m_vecData[idx1])] = idx1;
	m_mapIdx2Pos[getKey(m_vecData[idx2])] = idx2;
}


template <typename DataT, typename KeyT>
class HeapTrait
{
public:
	bool cmpVal_FZ(const DataT& data_F, const DataT& data_Z)  const {
		return cmpVal(data_F, data_Z);
	}
	KeyT key(const DataT& data) {
		return getKey(data);
	}
};

template <typename DataT, typename KeyT>
class HeapTrait_r
{
public:
	bool cmpVal_FZ(const DataT& data_F, const DataT& data_Z)  const {
		return !cmpVal(data_F, data_Z);
	}
	KeyT key(const DataT& data) {
		return getKey(data);
	}
};

#define DBG_HeapT2 
#ifdef DBG_HeapT2

#include <stdlib.h> 
#include <time.h> 

void test_HeapT2();

#endif

#endif
