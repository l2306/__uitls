
//	HashMap由数组+链表组成的，
//		数组是HashMap的主体，链表则是主要为了解决哈希冲突而存在的，
//		若定位到的数组位置不含链表（）,对于查找，添加等操作很快，仅需一次寻址即可；
//		若定位到的数组包含链表，对于添加操作，其时间复杂度为O(n)，首先遍历链表，存在即覆盖，否则新增；
//			对于查找操作来讲，仍需遍历链表，然后通过key对象的equals方法逐一比对查找。
//	所以，性能考虑，HashMap中的链表出现越少，性能才会越好

#include "_hashmap.h"

// 计算超过 capacity 的最小 2^n 
size_t getNextPow2(size_t capacity) {
	size_t ssize = 1;
	while (ssize < capacity) {
		ssize <<= 1;
	}
	return ssize;
}


#ifdef	TEST_HASHMAP

//首先要定义hash值函数与键值比较函数
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

int test_hashmap()
{
	HashMap<string, string, HashFunc, EqualKey> hashmap(100,"z_z");
	cout << " getTableSize(7)" << getNextPow2(7) << endl;

	hashmap.add("1", "111");
	hashmap.add("2", "222");
	hashmap.add("3", "333");

	cout << "after add:" << endl;
	cout << hashmap.get("1").c_str() << endl;
	cout << hashmap.get("2").c_str() << endl;
	cout << hashmap["3"].c_str() << endl;



	if (hashmap.del("1"))
		cout << "\"1\" is del" << endl;
	cout << hashmap.get("1").c_str() << endl;

	hashmap["2"] = "777";
	cout << hashmap["2"].c_str() << endl;

	HashFunc* fn = new HashFunc();
	cout << (*fn)("1") <<endl;
//
	cout << "------------" <<endl;
	HashMap<string, int*, HashFunc, EqualKey> has(100,(int*)NULL);
	int i=1111;
	has.add("1", &i);
	if( NULL==has.get("0"))
		cout <<" NULL "<<endl;
	cout << *has.get("1")<<endl;

	cin.get();
	return 0;
}

#endif
