#ifndef _POOL_ANY_OBJ_
#define _POOL_ANY_OBJ_
#include <functional>
#include <string>
#include <tuple>
#include <map>
#include <memory>
#include <typeindex>

#include <iostream>

#define _test_pool_any_obj_

using namespace std;

struct Any
{
	Any(void)               : m_typIdx(std::type_index(typeid(void))) {}
	Any(const Any& that)    : m_ptr(that.Clone()), m_typIdx(that.m_typIdx) {}
	Any(Any && that)        : m_ptr(std::move(that.m_ptr)), m_typIdx(that.m_typIdx) {}

	//创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
	template<typename U, 
		class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
	Any(U && value): 
		m_ptr(new Derived < typename std::decay<U>::type>(forward<U>(value))),
		m_typIdx(type_index(typeid(typename std::decay<U>::type))) {
	}

	bool IsNull() const { return !bool(m_ptr); }

	template<class U> bool Is() const
	{
		return m_typIdx == type_index(typeid(U));
	}

	//将Any转换为实际的类型
	template<class U>
	U& AnyCast()
	{
		if (!Is<U>()) {
			cout << "can not cast " << typeid(U).name() << " to " << m_typIdx.name() << endl;
			throw bad_cast();
		}
		auto derived = dynamic_cast<Derived<U>*> (m_ptr.get());
		return derived->m_value;
	}

	Any& operator=(const Any& a)
	{
		if (m_ptr == a.m_ptr)
			return *this;
		m_ptr = a.Clone();
		m_typIdx = a.m_typIdx;
		return *this;
	}

private:
	struct 							Base;
	typedef std::unique_ptr<Base>	BasePtr;

	struct Base {
		virtual ~Base() {}
		virtual BasePtr Clone() const = 0;
	};

	template<typename T>
	struct Derived : Base
	{
		template<typename U>
		Derived(U && value)
			: m_value(forward<U>(value)) {
		}
		BasePtr Clone() const {
			return BasePtr(new Derived<T>(m_value));
		}
		T m_value;
	};

	BasePtr Clone() const
	{
		if (m_ptr != nullptr)
			return m_ptr->Clone();
		return nullptr;
	}

	BasePtr 				m_ptr;
	std::type_index			m_typIdx;
};

class Pool_AnyObj
{
	const int MaxObjectNum = 10;
	typedef struct { int cur;	int max; }obj_cnt;
	template<typename T, typename... Args>
	using Constructor = std::function<std::shared_ptr<T>(Args...)>;
public:

	Pool_AnyObj() : needClear(false) {
	}

	~Pool_AnyObj() {
		needClear = true;
		cout << __FUNCTION__ << endl;
	}

	template<typename T, typename... Args>
	void Create(int num, Args... args)
	{
		if (num <= 0 || num > MaxObjectNum)
			throw std::logic_error("object num errer");

		auto constructName = typeid(Constructor<T, Args...>).name();

		Constructor<T, Args...> f = [constructName, this](Args... args) {
			return createPtr<T>(string(constructName), args...);
		};

		m_map.emplace(typeid(T).name(), f);
		m_counter.emplace(constructName, obj_cnt{ num,num });
		for (int i = 0;i < num;i++)
			m_object_map.emplace(constructName, createPtr<T, Args...>(string(constructName), args...));

	}

	template<typename T, typename... Args>
	std::shared_ptr<T> createPtr(std::string& constructName, Args... args)
	{
		return std::shared_ptr<T>(new T(args...), [constructName, this](T* t) {
			if (needClear)
				delete[] t;
			else {
				if (m_counter[constructName].cur == m_counter[constructName].max)
					delete[] t;
				else {
					++m_counter[constructName].cur;
					m_object_map.emplace(constructName, std::shared_ptr<T>(t));
				}
			}
		});
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> Get(Args... args)
	{
		using ConstructType = Constructor<T, Args...>;

		std::string constructName = typeid(ConstructType).name();
		auto range = m_map.equal_range(typeid(T).name());
		//auto range = m_map.equal_range(constructName);

		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second.Is<ConstructType>())
			{
				auto ptr = GetInstance<T>(constructName, args...);

				if (ptr != nullptr)
					return ptr;

				return CreateInstance<T, Args...>(it->second, constructName, args...);
			}
		}

		return nullptr;
	}

private:
	template<typename T, typename... Args>
	std::shared_ptr<T> CreateInstance(Any& any, std::string& constructName, Args... args)
	{
		using ConstructType = Constructor<T, Args...>;
		ConstructType f = any.AnyCast<ConstructType>();

		return createPtr<T, Args...>(constructName, args...);
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> GetInstance(std::string& constructName, Args... args)
	{
		auto it = m_object_map.find(constructName);
		if (it == m_object_map.end())
			return nullptr;

		auto ptr = it->second.AnyCast<std::shared_ptr<T>>();
		//   if (sizeof...(Args)>0)					//存在构造参数， 必须重建才能将构造参数传入
		//      *ptr.get() = std::move(T(args...));

		--m_counter[constructName].cur;
		m_object_map.erase(it);
		return ptr;
	}

	//template<typename T, typename... Args>
	//void InitPool(T& f, std::string& constructName, Args... args)
	//{
	//    int num = m_counter[constructName];

	//    if (num != 0)
	//    {
	//        for (int i = 0; i < num - 1; i++)
	//        {
	//            m_object_map.emplace(constructName, f(args...));
	//        }
	//        //m_counter[constructName] = 0;
	//    }
	//}

private:
	std::multimap<std::string, Any> m_map;
	std::multimap<std::string, Any> m_object_map;
	std::map<std::string, obj_cnt>	m_counter;
	bool							needClear;
};

#ifdef _test_pool_any_obj_

#include <string>

void TestAny()
{
	Any n;
	auto r = n.IsNull();            //true
	std::string s1 = "hello";
	n = s1;
	n = "world";
	n.AnyCast<int>();               //can not cast int to string
	Any n1 = 1;
	n1.Is<int>();                   //true
}

struct T_A
{
	T_A() {
		cout << this << endl << __FUNCTION__ << endl;
	}
	T_A(int a, int b) :m_a(a), m_b(b) {
		cout << this << endl << __FUNCTION__ << "	(int, int)" << endl;
	}
	void Fun() {
		cout << this << endl << hex << m_a << " " << m_b++ << endl;
	}
	int m_a = 0;
	int m_b = 0;
};

struct T_BB
{
	T_BB() {
		cout << this << endl << __FUNCTION__ << endl;
	}
	void Fun() {
		cout << this << endl << __FUNCTION__ << "" << endl;
	}
};

void TestPool_AnyObj(Pool_AnyObj& pool)
{
	pool.Create<T_A>(2);
	pool.Create<T_A, int, int>(2, 1, 1);			//具体实现 
	pool.Create<T_BB>(3);

	{
		auto p = pool.Get<T_A>();
		p->Fun();
	}
	auto p = pool.Get<T_A>();
	p->Fun();

	auto pb = pool.Get<T_BB>();
	pb->Fun();
	{
		auto pb = pool.Get<T_BB>();
		pb->Fun();
	}

	int a = 3, b = 4;
	auto p3 = pool.Get<T_A>(a, b);
	cout << p3 << endl;
	{
		auto p3 = pool.Get<T_A>(0x33, 4);
		p3->Fun();
		{
			auto p3 = pool.Get<T_A>(0x333, 4);
			p3->Fun();
			{
				auto p3 = pool.Get<T_A>(0x3333, 4);
				p3->Fun();
				{
					auto p3 = pool.Get<T_A>(0x33333, 4);
					p3->Fun();
				}
			}
		}
	}
	{
		auto p3 = pool.Get<T_A>(a + 6, b);
		p3->Fun();
		cout << p3 << endl;
	}

	//auto p0 = pool.Get<T_A>();
	//p0->Fun();
	//cout << p0 << endl;
}

#endif


//std::shared_ptr<int> get() {
//	return	std::shared_ptr<int>(new int, [](int* t) {
//		printf("get sp \n");
//	});
//}
//
//////core 原因临时变量销毁时
////std::shared_ptr<int>& get_ref() {
////	return	std::shared_ptr<int>(new int, [](int* t) {
////		printf("get sp \n");
////	});
////}
//
//std::shared_ptr<int> p = shared_ptr<int>(new int, [](int* t) {
//	printf("ooo \n");
//});
//
//std::shared_ptr<int>(new int, [](int* t) {
//	printf("xxx \n");
//});
//
//std::shared_ptr<int>  sp2 = get();

#endif // _pool_any_obj_