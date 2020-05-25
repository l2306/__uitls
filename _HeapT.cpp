
#include "_HeapT.h"

class Test1
{
public:
	Test1(int idx, int num)
		:idx(idx), num(num) {}
	~Test1() {}
	friend int getKey(Test1* const& data);
	friend bool cmpVal(Test1* const& data_P, Test1* const& data_S);
	friend ostream& operator<< (ostream& os, Test1* const& p);
	//private: 
	int		idx;
	int		num;
};
bool cmpVal(Test1* const& data_P, Test1* const& data_S) {
	return data_P->num < data_S->num;
}
int getKey(Test1* const& data) {
	return data->idx;
}
ostream& operator<< (ostream& os, Test1* const& p)
{
	os << "idx: " << p->idx << " num: " << p->num ;
	return os;
}


class Test2
{
public:
	Test2(string idx, int num)
		:idx(idx), num(num){}
	~Test2() {}
	friend string getKey(Test2* const& data);
	friend bool cmpVal(Test2* const& data_P, Test2* const& data_S);
	friend ostream& operator<< (ostream& os, Test2* const& p);
	//private: 
	string	idx;
	int		num;
};
bool cmpVal(Test2* const& data_P, Test2* const& data_S) {
	return data_P->num < data_S->num;
}
string getKey(Test2* const & data) {
	return data->idx;
}
ostream& operator<< (ostream& os, Test2* const& p)
{
	os << "idx: " << p->idx << " num: " << p->num ;
	return os;
}


void test_HeapT2()
{
	{
		HeapT2<Test1*, int, HeapTrait<Test1*, int> > hp;
		hp.insert(new Test1(5, 5));
		hp.insert(new Test1(2, 2));
		hp.insert(new Test1(4, 4));
		hp.insert(new Test1(9, 9));
		hp.insert(new Test1(7, 7));
		hp.insert(new Test1(3, 3));
		hp.insert(new Test1(1, 1));
		hp.insert(new Test1(8, 8));
		Test1*  my = new Test1(6, 6);
		hp.insert(my);

		srand((unsigned)time(NULL));
		for (int i = 0; i < 10;i++)
		{
			my->num = rand() % 20-10;
			hp.update(my);
			cout << "  update  " << my << ends << ends << " \n\t\t\t top  " << hp.top() << endl;
		}

		while (hp.size())
		{
			bool b = false;
			Test1*	top = hp.top(&b);
			if (b) {
				cout << top << endl;
				hp.remove(hp.top());
			}
			else
				cout << "empty" << endl;
		}
	}
    {
		HeapT2<Test2*, string, HeapTrait_r<Test2*, string> > hp;
		hp.insert(new Test2("5", 5));
		hp.insert(new Test2("2", 2));
		hp.insert(new Test2("4", 4));
		hp.insert(new Test2("9", 9));
		hp.insert(new Test2("7", 7));
		hp.insert(new Test2("3", 3));
		hp.insert(new Test2("1", 1));
		hp.insert(new Test2("8", 8));
		Test2*  my0 = new Test2("0", 6);
		hp.insert(my0);
		Test2*  my = new Test2("6", 6);
		hp.replace(my0,my);

		srand((unsigned)time(NULL));
		for (int i = 0; i < 10;i++)
		{
			my->num = rand() % 20;
			hp.update(my);
			cout << "  update  " << my << ends << ends << " \n\t\t\t top  " << hp.top() << endl;
		}

		while (hp.size())
		{
			bool b = false;
			Test2*	top = hp.top(&b);
			if (b) {
				cout << top << endl;
				hp.remove(hp.top());
			}
			else
				cout << "empty" << endl;
		}
	}
}


