#ifndef _singleton_H_
#define _singleton_H_

template<typename T>
class Singleton
{
	struct object_creator{
		object_creator(){
			Singleton<T>::instance();
		}
		inline void do_nothing() const {}
	};
	static object_creator create_object;

public:
	typedef T object_type;
	static T& instance(){
		static T obj;
		create_object.do_nothing();	//do_nothing确保create_object构造函数被调用 跟模板的编译有关
		return obj;
	}
};
template<typename T>
typename Singleton<T>::object_creator    Singleton<T>::create_object;

/*
#ifndef _test_singleton_H_
#define _test_singleton_H_

//#include "_singleton.h"
#include <iostream>

class C_single
{
protected:
	C_single() {}
	~C_single() {}
	friend class Singleton<class C_single>;
public:
	void do_something() {
		std::cout <<__LINE__<<" " << __FUNCTION__ <<"\n";
	};
};
class C_single2
{
protected:
	C_single2() {}
	~C_single2() {}
	friend class Singleton<class C_single2>;
public:
	void do_something() {
		std::cout <<__LINE__<<" "<< __FUNCTION__ <<"\n";
	};
};

int test_single()
{
	Singleton<class C_single>::instance().do_something();
	std::cout << &Singleton<C_single>::instance() << std::endl;
	std::cout << &Singleton<C_single2>::instance() << std::endl;
	std::cout << &Singleton<C_single2>::instance() << std::endl;
	std::cout << &Singleton<class C_single>::instance() << std::endl;
	std::cout << &Singleton<class C_single>::instance() << std::endl;
	return 0;
}
#endif
*/

#endif // !
