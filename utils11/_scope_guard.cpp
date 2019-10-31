
#include <utility>
#include <functional>

//c++11 实现的ScopeGuard
//  ScopeGuard
//      用dismiss实现 正常结束时执行，只有异常抛出发生或没有正常退出时释放资源。
//
//　　当程序没有发生异常正常退出时，需要调用一下dismiss函数，以解除ScopeGuard，
//    当程序异常退出时，会进入异常处理函数去释放相应资源，实现ScopeGuard的目的。

template <typename F>
class ScopeGuard
{
public:
    explicit ScopeGuard(F && f)     : _fun(std::move(f)), _can(true){}
    explicit ScopeGuard(const F& f) : _fun(f), _can(true){}
    ScopeGuard(ScopeGuard && rhs)   : _fun(std::move(rhs._fun)), _can(rhs._can)
                            { rhs.dismiss(); }
    ~ScopeGuard()           { if (_can) _func(); }
    void dismiss()          { _can = false;}

private:
    F       		        _fun;
    bool    		        _can;
    ScopeGuard()                              = delete;;
    ScopeGuard(const ScopeGuard&)             = delete;;
    ScopeGuard& operator=(const ScopeGuard&)  = delete;;

//template<class... Args>
//auto Run(Args&&... args)->typename std::result_of<F(Args...)>::type
//{
//　　return _func(std::forward<Args>(args)...);
//}

};

template <typename F>
ScopeGuard<typename std::decay<F>::type> makeScopeGuard(F && f)
{
    return ScopeGuard<typename std::decay<F>::type>(std::forward<F>(f));
}

#include <iostream>
using namespace std;

void testScopeGuard()
{    
    std::function < void()> f = [] {
	 cout << "cleanup from unnormal exit" << endl; 
    };
    {        
        auto gd = makeScopeGuard(f);
        //...
        gd.dismiss();               //正常退出 ,摒弃执行
    }
    {
        auto gd = makeScopeGuard(f);
        //...
        throw 1;                    //异常退出
    }
    {
        auto gd = makeScopeGuard(f);
        return;                     //非正常退出
        //...
    }
}


int main(){
	try {
		 testScopeGuard();
	} catch (int i) {
		cout << "  catch throw " << i<<endl;
	};
   
    return 0;
}
