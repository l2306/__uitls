
#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <iostream>
using namespace std;

#define TEST_HASHMAP
#ifdef	 TEST_HASHMAP
int test_hashmap();
#endif

// 计算超过或等于capacity的最小 2^n 
size_t getNextPow2(size_t capacity);

template<class Key, class Val>
class HashNode;

template <class Key, class Val, class HashFunc, class EqualKey>
class HashMap
{
public:
	HashMap(int size,const Val val);
	~HashMap();
	bool add(const Key& key, const Val& val);
	bool del(const Key& key);
	Val& get(const Key& key);
	Val& operator [](const Key& key);
	void clear();
private:
	HashFunc				hash;
	EqualKey				equal;
	unsigned int			_size;
	HashNode<Key, Val>	**table;
	Val					    nullValue;
};

template<class Key, class Val>
class HashNode
{
public:
	Key			_key;
	Val			_val;
	HashNode*	next;

	HashNode(Key key, Val val)
	{
		_key	= key;
		_val	= val;
		next	= NULL;
	}
	~HashNode()
	{
	}
	HashNode& operator=(const HashNode& node)
	{
		_key	= node.key;
		_val	= node.val;
		next	= node.next;
		return *this;
	}
};

template <class Key, class Value, class HashFunc, class EqualKey>
HashMap<Key, Value, HashFunc, EqualKey>::HashMap(int size, Value nullVal)
	: nullValue(nullVal)
{
	hash        = HashFunc();
	equal       = EqualKey();
	_size       = getNextPow2(size);
	table       = new HashNode<Key, Value>*[_size];
	for (unsigned i = 0; i < _size; i++)
		table[i] = NULL;
}

template <class Key, class Value, class HashFunc, class EqualKey>
HashMap<Key, Value, HashFunc, EqualKey>::~HashMap()
{
	clear();
	delete table;
}

template <class Key, class Value, class HashFunc, class EqualKey>
void HashMap<Key, Value, HashFunc, EqualKey>::clear()
{
	for (unsigned i = 0; i < _size; i++)
	{
		HashNode<Key, Value> *currentNode = table[i];
		while (currentNode)
		{
			HashNode<Key, Value> *temp = currentNode;
			currentNode = currentNode->next;
			delete temp;
		}
	}
}

template <class Key, class Value, class HashFunc, class EqualKey>
bool HashMap<Key, Value, HashFunc, EqualKey>::add(const Key& key, const Value& value)
{
	int                     index = hash(key) & _size;
	HashNode<Key, Value>*   node = new HashNode<Key, Value>(key, value);
	node->next              = table[index];
	table[index]            = node;
    node                    = node->next;
    HashNode<Key, Value>*   prev = NULL;
	while (node)
	{
		if (node->_key == key)
        {
			if (prev == NULL)
				table[index] = node->next;
			else
				prev->next = node->next;
			delete node;
			return true;
		}
		prev = node;
		node = node->next;
	}
	return true;
}

template <class Key, class Value, class HashFunc, class EqualKey>
bool HashMap<Key, Value, HashFunc, EqualKey>::del(const Key& key)
{
	unsigned                index = hash(key) & _size;
	HashNode<Key, Value>*   node = table[index];
	HashNode<Key, Value>*   prev = NULL;
	while (node)
	{
		if (node->_key == key)
        {
			if (prev == NULL)
				table[index] = node->next;
			else
				prev->next = node->next;
			delete node;
			return true;
		}
		prev = node;
		node = node->next;
	}
	return false;
}

template <class Key, class Value, class HashFunc, class EqualKey>
Value& HashMap<Key, Value, HashFunc, EqualKey>::get(const Key& key)
{
	unsigned  index = hash(key) & _size;
	if (table[index] == NULL)
		return nullValue;
	else
	{
		HashNode<Key, Value> * node = table[index];
		while (node)
		{
			if (node->_key == key)
				return node->_val;
			node = node->next;
		}
		return nullValue;
	}
}

template <class Key, class Value, class HashFunc, class EqualKey>
Value& HashMap<Key, Value, HashFunc, EqualKey>::operator [](const Key& key)
{
	return get(key);
}


/*
//比较取模 数值取并集的效率

#include <stdio.h>  
#include <windows.h>  
#include <time.h> //time_t time()  clock_t clock()  
#include <Mmsystem.h>             //timeGetTime()  
#pragma comment(lib, "Winmm.lib")   //timeGetTime()  

//位运算(&)效率要比代替取模运算(%)高很多，
//	主要原因是位运算直接对内存数据进行操作，不需要转成十进制，因此处理速度非常快
int test_4_win()
{
	{
		LARGE_INTEGER  large_interger;
		double dff;
		__int64  c1, c2;
		QueryPerformanceFrequency(&large_interger);
		dff = large_interger.QuadPart;
		QueryPerformanceCounter(&large_interger);
		c1 = large_interger.QuadPart;
		for (size_t i = 0; i < 100000; i++)
		{
			int tt = i % 256;
		}
		QueryPerformanceCounter(&large_interger);
		c2 = large_interger.QuadPart;
		printf("本机高精度计时器频率%lf\n", dff);
		printf("第一次计时器值%I64d 第二次计时器值%I64d 计时器差%I64d\n", c1, c2, c2 - c1);
		printf("计时%lf毫秒\n", (c2 - c1) * 1000 / dff);
	}
	cout << "" << endl << endl << endl << endl;
	{
		LARGE_INTEGER  large_interger;
		double dff;
		__int64  c1, c2;
		QueryPerformanceFrequency(&large_interger);
		dff = large_interger.QuadPart;
		QueryPerformanceCounter(&large_interger);
		c1 = large_interger.QuadPart;
		for (size_t i = 0; i < 100000; i++)
		{
			int tt = (i & (2 ^ 8 - 1));
		}
		QueryPerformanceCounter(&large_interger);
		c2 = large_interger.QuadPart;
		printf("本机高精度计时器频率%lf\n", dff);
		printf("第一次计时器值%I64d 第二次计时器值%I64d 计时器差%I64d\n", c1, c2, c2 - c1);
		printf("计时%lf毫秒\n", (c2 - c1) * 1000 / dff);
	}
}
*/

/*
//增加高位扰动
size_t getHash(size_t h) {
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}
*/
/*
static int indexFor(int h, int length) {
	return h & (length - 1);
}
*/

/**
* hash算法仿函数
*/
/*
template<class KeyType>
struct cache_hash_func {
};

inline std::size_t cache_hash_string(const char* __s) {
	unsigned long __h = 0;
	for (; *__s; ++__s)
		__h = 5 * __h + *__s;
	return std::size_t(__h);
}

template<>
struct cache_hash_func<std::string> {
	std::size_t operator()(const std::string & __s) const {
		return cache_hash_string(__s.c_str());
	}
};

template<>
struct cache_hash_func<char*> {
	std::size_t operator()(const char* __s) const {
		return cache_hash_string(__s);
	}
};

template<>
struct cache_hash_func<const char*> {
	std::size_t operator()(const char* __s) const {
		return cache_hash_string(__s);
	}
};
*/

#endif
