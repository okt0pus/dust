#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include <iostream>

namespace
{
// type selection hierarchy
namespace
{
// type promotion next in hierarchy
template<std::size_t S, bool B> struct _next_up {};

template<> struct _next_up<1U, true>    { typedef int16_t  type; };
template<> struct _next_up<1U, false>   { typedef uint16_t type; };

template<> struct _next_up<2U, true>    { typedef int32_t  type; };
template<> struct _next_up<2U, false>   { typedef uint32_t type; };

template<> struct _next_up<4U, true>    { typedef int64_t  type; };
template<> struct _next_up<4U, false>   { typedef uint64_t type; };
#ifdef __SIZEOF_INT128__
template<> struct _next_up<8, true>     { typedef __int128_t  type; };
template<> struct _next_up<8, false>    { typedef __uint128_t type; };
#else
template<> struct _next_up<8, true> { typedef int64_t  type; };
template<> struct _next_up<8, false> { typedef uint64_t type; };
#endif
}

//// integral check
//template<typename U, typename ...T>
//struct integral_types {
//    static constexpr bool value = (std::is_integral<typename std::decay<U>::type>::value && integral_types<T...>::value);
//};
//
//template<typename T>
//struct integral_types<T> {
//    static constexpr bool value = std::is_integral<typename std::decay<T>::type>::value;
//};
    
// one of the types is signed check
template<typename U, typename ...T>
struct one_signed_types {
    static constexpr bool value = (std::is_signed<typename std::decay<U>::type>::value || one_signed_types<T...>::value);
};

template<typename T>
struct one_signed_types<T> {
    static constexpr bool value = std::is_signed<typename std::decay<T>::type>::value;
};

// get maximum sizeof
template<typename U, typename ...T>
struct max_sizeof {
    static constexpr std::size_t value = std::max(sizeof(typename std::decay<U>::type), max_sizeof<T...>::value);
};

template<typename T>
struct max_sizeof<T> {
    static constexpr std::size_t value = sizeof(typename std::decay<T>::type);
};

// next type for promotion
template<typename U, typename... T>
struct next_up {
    typedef typename _next_up<max_sizeof<U, T...>::value, one_signed_types<U, T...>::value>::type type;
};

template <typename... T>
using next_up_t = typename next_up<T...>::type;
}

template<
    typename T,
    typename std::enable_if<!std::is_const<T>::value, typename std::decay<T>::type>::type
        min = std::numeric_limits<T>::lowest(),
    typename std::enable_if<std::is_integral<T>::value, typename std::decay<T>::type>::type
        max = std::numeric_limits<T>::max()>
class xint_sat_t
{
public:
    typedef typename std::decay<T>::type type;

    static const type min_val = min;
    static const type max_val = max;

private:
    template<typename U>
    using same_type = std::is_same<typename std::decay<U>::type, type>;

    T value;

public:
    constexpr xint_sat_t() noexcept : value{ static_cast<type>(0) } {}

    template<
        typename U,
        typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr>
    constexpr explicit xint_sat_t(const U& val) noexcept : value{ clamp(val) } {}

    constexpr operator const T& () const noexcept { return value; }
    constexpr operator T& () noexcept { return value; }

    template<typename U>
    constexpr decltype(auto) operator+(const U& other) const noexcept { return __add(value, other); }
    template<typename U>
    constexpr decltype(auto) operator-(const U& other) const noexcept { return __sub(value, other); }

    // TODO fix passing xint_sat_t to clamp
    template<
        typename U,
        typename std::enable_if<same_type<U>::value>::type* = nullptr>
    static constexpr type clamp(const U& val) noexcept { return static_cast<type>(val); }

    template<
        typename U,
        typename std::enable_if<std::is_floating_point<U>::value>::type* = nullptr>
        static constexpr type clamp(const U& val) noexcept
    {
        const U _tmp = (val >= 0) ? (val + 0.5f) : (val - 0.5f);
        constexpr U minv = static_cast<U>(min_val);
        constexpr U maxv = static_cast<U>(max_val);
        return static_cast<type>(std::max(minv, std::min(maxv, _tmp)));
    }

    template<
        typename U,
        typename std::enable_if<!same_type<U>::value&& std::is_integral<U>::value>::type* = nullptr>
        static constexpr type clamp(const U& val) noexcept
    {
        constexpr U minv = static_cast<U>(min_val);
        constexpr U maxv = static_cast<U>(max_val);
        return static_cast<type>(std::max(minv, std::min(maxv, val)));
    }
private:

    template<
        typename UA,
        typename UB,
        typename std::enable_if<std::is_arithmetic<UA>::value || std::is_arithmetic<UB>::value>::type* = nullptr>
    static constexpr xint_sat_t __add(const UA& a, const UB& b) noexcept
    {
        using larger_type = typename std::make_signed<next_up_t<std::decay<UA>::type, std::decay<UB>::type, type>>::type;
        const larger_type _sum = (static_cast<larger_type>(a) + static_cast<larger_type>(b));
        return xint_sat_t(_sum);
    }

    template<
        typename UA,
        typename UB,
        typename std::enable_if<std::is_arithmetic<UA>::value || std::is_arithmetic<UB>::value>::type* = nullptr>
    static constexpr xint_sat_t __sub(const UA& a, const UB& b) noexcept
    {
        using larger_type = typename std::make_signed<next_up_t<std::decay<UA>::type, std::decay<UB>::type, type>>::type;
        const larger_type _sum = (static_cast<larger_type>(a) - static_cast<larger_type>(b));
        return xint_sat_t(_sum);
    }
};

namespace std
{

template<typename T, T _min, T _max>
class decay<xint_sat_t<T, _min, _max>>
{
public:
    typedef typename decay<T>::type type;
};

}


