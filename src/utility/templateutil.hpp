#ifndef TEMPLATE_UTIL_HPP
#define TEMPLATE_UTIL_HPP

#include <stdint.h>

namespace util
{

template<typename Type, Type... Args> class TestInSet
{
public:
    template<Type Check> static bool test() {return false;}
};

template<typename Type, Type T1, Type... Args> class TestInSet<Type,T1,Args...>
{
public:
    template<Type Check> static bool test() {return Check == T1 || TestInSet<Type,Args...>::template test<Check>();}
};

template<typename Type, Type T1> class TestInSet<Type,T1>
{
public:
    template<Type Check> static bool test() {return Check == T1;}
};

template<unsigned long... Args> struct BuildMask
{static constexpr uint64_t mask = 0;};
template<unsigned long Arg> struct BuildMask<Arg>
{
    static constexpr uint64_t mask = 1ULL << Arg;
    static constexpr bool test(const unsigned long tval) {return Arg == tval;}
};
template<unsigned long Arg, unsigned long... Args> struct BuildMask<Arg,Args...>
{
    static constexpr uint64_t mask = (1ULL << Arg) | BuildMask<Args...>::mask;
    static constexpr bool test(const unsigned long tval) {return (mask & (1ULL << tval));}
};

}

#endif

