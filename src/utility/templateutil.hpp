#ifndef TEMPLATE_UTIL_HPP
#define TEMPLATE_UTIL_HPP

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
{static constexpr unsigned long mask = 0;};
template<unsigned long Arg> struct BuildMask<Arg>
{static constexpr unsigned long mask = 1 << Arg;};
template<unsigned long Arg, unsigned long... Args> struct BuildMask<Arg,Args...>
{static constexpr unsigned long mask = (1 << Arg) | BuildMask<Args...>::mask;};

}

#endif

