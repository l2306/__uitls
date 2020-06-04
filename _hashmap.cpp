
//	HashMap������+������ɵģ�
//		������HashMap�����壬����������ҪΪ�˽����ϣ��ͻ�����ڵģ�
//		����λ��������λ�ò���������,���ڲ��ң���ӵȲ����ܿ죬����һ��Ѱַ���ɣ�
//		����λ���������������������Ӳ�������ʱ�临�Ӷ�ΪO(n)�����ȱ����������ڼ����ǣ�����������
//			���ڲ��Ҳ��������������������Ȼ��ͨ��key�����equals������һ�ȶԲ��ҡ�
//	���ԣ����ܿ��ǣ�HashMap�е��������Խ�٣����ܲŻ�Խ��

#include "_hashmap.h"

// ���㳬�� capacity ����С 2^n 
size_t getNextPow2(size_t capacity) {
	size_t ssize = 1;
	while (ssize < capacity) {
		ssize <<= 1;
	}
	return ssize;
}


#ifdef	TEST_HASHMAP

//����Ҫ����hashֵ�������ֵ�ȽϺ���
class HashFunc
{
public:
	int operator()(const string & key)
	{
		int hash = 0;		//�˹�ϣȡֵ���� ���Ǻܺ�
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
