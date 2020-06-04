
#ifndef _THREADPOOL_
#define _THREADPOOL_

#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <iostream>

// 异步流水线程池		可以随意添加任务

// 命名空间
namespace utils {
	class TaskExec;
}

//	对象初始化方式采用 {}，而不使用之前的 () 

class utils::TaskExec {
	using Task = std::function<void()>;

private:

	std::vector<std::thread>		pool;	// 线程池
	std::queue<Task>			tasks;// 任务队列
	std::mutex				m_task;// 同步
	std::condition_variable		cv_task;
	std::atomic<bool>			stop;	// 是否关闭提交

public:
	// 构造
	TaskExec(size_t size = 4) : stop{ false } {
		size = size < 1 ? 1 : size;
		for (size_t i = 0; i < size; ++i) {
			pool.emplace_back(&TaskExec::schedual, this);
			//thread{&TaskExecutor::schedual, this} 
			//	构造了一个线程对象，执行函数是成员函数 TaskExec::schedual
			//emplace_back与push_back 效果一样 前者更优
		}
	}

	// 析构
	~TaskExec() {
		for (std::thread& thread : pool) {
			thread.detach(); // 让线程“自生自灭”
			//thread.join(); // 等待任务结束， 前提：线程一定会执行完
		}
	}

	// 停止任务提交
	void shutdown() {
		this->stop.store(true);
	}

	// 重启任务提交
	void restart() {
		this->stop.store(false);
	}

	// 提交一个任务
	template<class F, class... Args>
	auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
	{
		// stop == true ??
		if (stop.load()) {		
			throw std::runtime_error("task executor have closed commit.");
		}
		
		// typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
		//delctype(expr) 用来推断 expr 的类型，和 auto 是类似的，相当于类型占位符，占据一个类型的位置；
		//auto f(A a, B b) -> decltype(a+b) 
		//	不能写作 decltype(a+b) f(A a, B b)  c++规定
		using RtnTyp = decltype(f(args...)); 
		auto task = std::make_shared<std::packaged_task<RtnTyp()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			); 
		// 添加任务到队列		// push(Task{...})
		std::lock_guard<std::mutex> lock{ m_task };
		tasks.emplace([task]() {			
			(*task)();
		});

		// 唤醒线程执行
		cv_task.notify_all();		

		std::future<RtnTyp> future = task->get_future();
		return future;
	}

private:
	// 获取一个待执行的 task
	Task get_one_task() {
		std::unique_lock<std::mutex> lock{ m_task };
		cv_task.wait(lock, [this]() { 
				return !tasks.empty(); }); // wait 直到有 task
		Task task{ std::move(tasks.front()) };		// 取一个 task
		tasks.pop();
		return task;
	}

	// 任务调度
	void schedual() {
		while (true) {
			if (Task task = get_one_task())
			{
				task(); //
			}else {
				//return; // done
			}
		}
	}
};

#ifndef _test_TaskExec_
#define _test_TaskExec_

void f()
{
	std::cout << "f() !" << std::endl;
}

struct G {
	int operator()() {
		std::cout << "G() !" << std::endl;
		return 99 ;
	}
};


int test_TaskExec() {
	try {
		zz::TaskExec executor{ 10 };

		std::future<void>	ff = executor.commit(f);		//
		std::future<int>	fg = executor.commit(G());
		std::future<std::string> fh = executor.commit(
			[]()->std::string
				{ std::cout << "[]() !" << std::endl; return "str_rtn!";}
		);

		executor.shutdown();

		ff.get();
		
		std::cout << fg.get() << " "<< fh.get() << std::endl;
		//std::cout << fh.get() << " "<< fg.get()  << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));

		executor.restart(); // 重启任务
//		for(int i=0; i<9;i++)
//			executor.commit(f).get(); //
		std::future<void>	tt = executor.commit(f);

		std::cout << "end...\t" << std::endl;
		return 0;
	}
	catch (std::exception& e) {
		std::cout << "err ...\t" << e.what() << std::endl;
		return -1;
	}
}

#endif

//using Task = function<void()> 是类型别名，简化了 typedef 的用法。
//	function<void()> 可以认为是一个函数类型，接受任意原型是 void() 的函数，或是函数对象，或是匿名函数。
//	void() 意思是不带参数，没有返回值

//packaged_task 就是任务函数的封装类，通过 get_future 获取 future ， 然后通过 future 可以获取函数的返回值(future.get())；packaged_task 本身可以像函数一样调用 () 

//lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()，是 c++ RAII 的 idea；
//condition_variable cv; 条件变量， 需要配合 unique_lock 使用；
//unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()。 
//cv.wait() 之前需要持有 mutex，wait 本身会 unlock() mutex，如果条件满足则会重新持有 mutex

//make_shared 构造 shared_ptr 智能指针 (引用为零时 自动析构)。
//	用法 shared_ptr<int> p = make_shared<int>(4) 然后 *p == 4 

//forward() 函数，类似于 move() 函数，后者是将参数右值化，前者是... 肿么说呢
//	大概意思就是：不改变最初传入的类型的引用类型(左值还是左值，右值还是右值)；

#endif

