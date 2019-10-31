#include <Windows.h>
#include <string>
#include <map>
#include <functional>

using namespace std;

// auto max = runFun<int(int, int)>("Max", 5, 8);
// auto ret = runFun<int(int)>("Get", 5);

class Parser_libdll
{
public:

    Parser_libdll() 	{ }
    ~Parser_libdll()	{ UnLoad();	}

    bool Load(const string& dllPath)
    {
        m_hMod = LoadLibrary(dllPath.data());
        if (nullptr == m_hMod) {
            printf("LoadLibrary failed\n");
            return false;
        }
        return true;
    }

    bool UnLoad()
    {
        if (m_hMod == nullptr)
            return true;
        auto b = FreeLibrary(m_hMod);
        if (!b)
            return false;
        m_hMod = nullptr;
        return true;
    }

    template <typename T>
    T* GetFunction(const string& funcName)
    {
        auto addr = GetProcAddress(m_hMod, funcName.c_str());
        return (T*) (addr);
    }

    template <typename T, typename... Args>
    typename std::result_of<std::function<T>(Args...)>::type runFun(const string& funcName, Args&&... args)
    {
        auto f = GetFunction<T>(funcName);
        if (f == nullptr) {
            string s = "can not find this function " + funcName;
            throw std::exception(s.c_str());
        }            
        return f(std::forward<Args>(args)...);
    }


private:
    HMODULE m_hMod;
    //std::map<string, FARPROC> m_map;
};