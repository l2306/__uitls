//#define MAKE_PAIR(text) std::pair<std::string, decltype(text)>{#text, text}
//
//template<typename T>
//constexpr static inline auto apply(T const & args)
//{
//    return args;
//}
//
//template<typename T, typename T1, typename... Args>
//constexpr static inline auto apply(T const & t, const T1& first, const Args&... args)
//{
//    return apply(std::tuple_cat(t, std::make_tuple(MAKE_PAIR(first))), args...);
//}
//
//#define META(...) auto meta(){ return apply(std::tuple<>(), __VA_ARGS__); }

//这个不行

#include <array>
#include <string>
#include <tuple>
template<size_t N>
std::array<std::string, N> split(const std::string& s, const char delimiter)
{
    size_t          start   = 0;
    size_t          end     = s.find_first_of(delimiter);
    size_t          i       = 0;
    std::array<std::string, N> output;
    while (end <= std::string::npos)
    {
        output[i++] = std::move(s.substr(start, end - start));
        if (end == std::string::npos)
            break;
        start = end + 2;
        end = s.find_first_of(delimiter, start);
    }
    return output;
}

template<size_t N, typename T>
static inline auto make(const std::array<std::string, N>&ar, size_t index, const T& args)
{
    return args;
}

template<size_t N, typename T, typename T1, typename... Args>
static inline auto make(const std::array<std::string, N>&ar, size_t index, T const & t, T1& first, Args&... args)
{
    return make(ar, index + 1, std::tuple_cat(t, std::make_tuple(std::pair<std::string, T1&>(ar[index], first))), args...);
}

#define VA_ARGS_NUM(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define META(...) auto meta(){                                      \
    auto ar = split<VA_ARGS_NUM(__VA_ARGS__)>(#__VA_ARGS__, ',');   \
    return make(ar, 0, std::tuple<>(), __VA_ARGS__);                \
}


struct person
{
    std::string name;
    int age;
    META(name, age) //定义一个支持变参的meta函数
};
int main()
{
    person p = {“tom”, 20};
    auto tp = p.meta();
    static_assert(std::is_same(std::tuple<std::pair<std::string, int>>, decltype(tp), “not same”);
    //在内存中是这样的 {{“name”:”tom”}, {“age”:20}};
    return 0;
}