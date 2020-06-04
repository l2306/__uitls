#ifndef Ref_Ptr
#define Ref_Ptr

#include <string>

template <typename TT>
class RefPtr;

//应用计数类	记录引用个数
template <typename TT>
class Ref {
private:
	Ref(TT* p) 
        :_ptr(p), _cnt(1)  { }
	~Ref()
        { if(_ptr)   delete _ptr; };
	friend class RefPtr<TT>;                //设 指针管理类为友元，使能修改本类
	void incr()     { _cnt++; };			//增加引用计数
	void decr()     { _cnt--; };			//减少引用计数
	int getCnt()    { return _cnt; };		//返回引用计数

private:
	int     _cnt;		        //引用计数
	TT*     _ptr;			    //对象指针
};

//
template <typename TT>
class RefPtr {
public:
	// 默认构造  NULL		不推荐使用，因为用再去拷贝对象，浪费性能
	//RefPtr()
	//	: _ref(new Ref<TT>(NULL)), _tag("") {
	//};

	RefPtr(TT *ptr) 
		: _ref(new Ref<TT>(ptr)), _tag("") {
	};

	RefPtr(const RefPtr<TT>& refptr) {
		_ref = refptr._ref;
		_ref->incr();
		_tag = "";
	}

	~RefPtr() {
		_ref->decr();
		if (_ref->getCnt() <= 0) {
			delete _ref;
		}
	};

	//重载操作符＝		//内部引用指向改变
	RefPtr& operator=(const RefPtr<TT>& rhs) {
		rhs._ref->incr();							//左值内 原计数器减一
		_ref->decr();								//右值内 原计数器加一
				
		if (_ref->getCnt() <= 0) {				
			delete _ref;							//原计数小于零 可以析构了
		}				
		_ref = rhs._ref;							//赋值为 新计数器
		return *this;
	};

	void setTag(const std::string value) {  _tag = value; };
    inline std::string getTag() {	return _tag;    };
    inline int getRefCnt()  { return _ref->getCnt(); }
	TT& operator*()         {	return *(_ref->_ptr);  };  // 指针为空时奔溃  推荐用get()
	TT* operator->()        {	return _ref->_ptr;     };
	TT* get()               {	return _ref->_ptr;  };

private:
	Ref<TT>	*	_ref;		//计数类
	std::string	_tag;		//标签名
};

//
//#include <iostream>
//using namespace std;
//
//int test_2() {
//	int *num = new int(11);
//	{
//		RefPtr<int> ptr1(num);
//		ptr1.setTag("ptr1");
//		{
//			RefPtr<int> ptr2(ptr1);
//			ptr2.setTag("ptr2");
//			{
//				int *num7 = new int(77);
//				RefPtr<int> ptr7(num7);
//				ptr2 = ptr7;
//				ptr2.setTag("ptr2 变");
//				{
//					*ptr1 = 22;
//					std::cout << "----------------获取引用计数 " << std::endl;
//					std::cout << "ptr1 have " << ptr1.getRefCnt() << "次 " << *ptr1.get() << ends << ptr1.get() << std::endl;
//					std::cout << "ptr2 have " << ptr2.getRefCnt() << "次 " << *ptr2.get() << ends << ptr2.get() << std::endl;
//					std::cout << "ptr2 have " << ptr7.getRefCnt() << "次 " << *ptr7.get() << ends << ptr7.get() << std::endl;
//				}
//			}
//		}
//	}
//	return 0;
//}


#endif /* Ref_Ptr */

