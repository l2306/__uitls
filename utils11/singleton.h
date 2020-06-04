//java集中单例实现	https://www.cnblogs.com/Ycheng/p/7169381.html

#ifndef _SINGLETON_
#define _SINGLETON_

#include <utility>
#include <string>
#include <iostream>

#define nullptr NULL

// -std=c++11 完美转发是 c++11的新属性

//懒汉模式  用时创建 
template <typename T>
class singleton
{
public:
	//可变参数进行完美转发
	template<typename... Args>
	static T* instance(Args&&... args) {
		if (g_instance == nullptr)
			g_instance = new T(std::forward<Args>(args)...);
		return g_instance ;
	}

	//可变参数进行完美转发
	template<typename... Args>
	static bool createInstance(Args&&... args) {
		if (g_instance == nullptr)
			g_instance = new T(std::forward<Args>(args)...);
		return g_instance != nullptr;
	}

	static void destroyInstance(void) {
		if (g_instance != nullptr) {
			delete g_instance;
			g_instance == nullptr;
		}
	}

	static T *instance(void) {
		if (g_instance == nullptr)
			g_instance = new T();
		return g_instance; 
	}
private:
	static T *g_instance;
};

template <typename T>
T* singleton<T>::g_instance = nullptr;

#endif


/*
int	test_1();
int	test_2();

int main()
{
	test_1();
	test_2();
	return 0;
}

using namespace std;
//-----------------------------------------使用方法

class St1{
public:
	St1(int num1 = 0, int num2 = 0) 
		:num1_(num1), num2_(num2) 
	{
		cout << "St1" << this << endl;
	};
	~St1(void) {};
	int sum(void) { return num1_ + num2_; }
private:
	int num1_;
	int num2_;
};
#define instance_St1 (*singleton<St1>::instance())

class St2 {
public:
	St2(int num1 = 0, int num2 = 0)
		:num1_(num1), num2_(num2)
	{
		cout << "St2" << this << endl;
	};
	~St2(void) {};
	int sum(void) { return num1_ + num2_; }
private:
	int num1_;
	int num2_;
};
#define instance_St2 (*singleton<St2>::instance())

int test_1(){
	if (!singleton<St1>::createInstance(11, 22)) {
		cout << "create singleton  is failed" << '\n';
		return -1;
	}
	cout << singleton<St1>::instance()->sum() << '\n';
	cout << singleton<St1>::instance() << endl;
}

int test_2() {

	if (!singleton<St1>::createInstance(123, 0)) {
		cout << "create singleton  is failed" << '\n';
		return -1;
	}
	cout << singleton<St1>::instance()->sum() << '\n';

	if (!singleton<St2>::instance(1111, 2222)) {
		cout << "create singleton  is failed" << '\n';
		return -1;
	}
	cout << singleton<St2>::instance()->sum() << '\n';

	cout << &instance_St1 << "  " << &instance_St2  << '\n';

	if (!singleton<St1>::instance(999, 0)) {
		cout << "create singleton  is failed" << '\n';
		return -1;
	}
	cout << singleton<St1>::instance()->sum() << '\n';

	cout << &instance_St1 << "  " << &instance_St2  << '\n';

	return 0;
}

*/
