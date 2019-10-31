#include <memory>

//PImpl(private implementation) 其主要作用是解开类的使用接口和实现的耦合。
// Impl声明在外也可以，但是不能防止被人调用
//		绝对不要头文件中定义，那就失去了其目的
class Container
{
public:
    Container (const size_t size);
    Container (const Container& other);
    Container& operator =(const Container& other);

    int& operator [] (const int index);
    const int& operator [] (const int index) const;

private:
    class Impl;			
    std::shared_ptr<Impl> impl_;
};

/*
#include <iostream>
int main()
{
	Container c(2);
	c[1]=11;
	std::cout<< c[1];
	return 0;
}
*/
