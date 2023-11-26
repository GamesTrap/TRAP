#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec2TypedefsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        static_assert(std::same_as<typename Vec::value_type, T>);
        static_assert(std::same_as<typename Vec::pointer, T*>);
        static_assert(std::same_as<typename Vec::const_pointer, const T*>);
        static_assert(std::same_as<typename Vec::reference, T&>);
        static_assert(std::same_as<typename Vec::const_reference, const T&>);
        static_assert(std::same_as<typename Vec::iterator, typename std::array<T, 2>::iterator>);
        static_assert(std::same_as<typename Vec::const_iterator, typename std::array<T, 2>::const_iterator>);
        static_assert(std::same_as<typename Vec::size_type, u32>);
        static_assert(std::same_as<typename Vec::difference_type, isize>);
        static_assert(std::same_as<typename Vec::reverse_iterator, typename std::array<T, 2>::reverse_iterator>);
        static_assert(std::same_as<typename Vec::const_reverse_iterator, typename std::array<T, 2>::const_reverse_iterator>);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec2ConstructorsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //Default constructor
        {
            constexpr Vec v{};

            static_assert(v.x() == T(0));
            static_assert(v.y() == T(0));
        }

        //Copy constructor
        {
            constexpr Vec v{T(2), T(5)};
            constexpr Vec vCopy(v);

            static_assert(vCopy.x() == T(2));
            static_assert(vCopy.y() == T(5));
        }

        //Move constructor
        {
        }

        //Scalar constructor
        {
            constexpr Vec v(T(5));

            static_assert(v.x() == T(5));
            static_assert(v.y() == T(5));
        }

        //Value constructor
        {
            constexpr Vec v(T(5), T(10));

            static_assert(v.x() == T(5));
            static_assert(v.y() == T(10));
        }

        //Conversion constructor
        {
            constexpr Vec v(u64(100), u8(5));

            static_assert(v.x() == T(100));
            static_assert(v.y() == T(5));
        }

        //Conversion Vec2 constructor
        {
            constexpr Vec v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
        }

        //Conversion Vec3 constructor
        {
            constexpr Vec v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
        }

        //Conversion Vec4 constructor
        {
            constexpr Vec v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2ConstructorsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //Default constructor
        {
            Vec v{};

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(0));
        }

        //Copy constructor
        {
            Vec v{T(2), T(5)};
            Vec vCopy(v);

            REQUIRE(vCopy.x() == T(2));
            REQUIRE(vCopy.y() == T(5));
        }

        //Move constructor
        {
            Vec v(T(5), T(10));
            Vec vMove(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
        }

        //Scalar constructor
        {
            Vec v(T(5));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(5));
        }

        //Value constructor
        {
            Vec v(T(5), T(10));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(10));
        }

        //Conversion constructor
        {
            Vec v(u64(100), u8(5));

            REQUIRE(v.x() == T(100));
            REQUIRE(v.y() == T(5));
        }

        //Conversion Vec2 constructor
        {
            TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
        }

        //Conversion Vec3 constructor
        {
            TRAP::Math::tVec3<f64> in(f64(10.0), f64(127.0), f64(80.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
        }

        //Conversion Vec4 constructor
        {
            TRAP::Math::tVec4<f64> in(f64(10.0), f64(127.0), f64(80.0), f64(1.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2AssignmentsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //Move assignment operator
        {
            Vec v(T(5), T(10));
            Vec vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
        }

        //Copy assignment operator
        {
            const Vec v(T(5), T(10));
            Vec vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == T(5));
            REQUIRE(vCopy.y() == T(10));
        }

        //Conversion copy assignment operator
        {
            TRAP::Math::tVec2<f64> v(f64(5.0), f64(10.0));
            Vec vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == T(5));
            REQUIRE(vConversion.y() == T(10));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec2AccessorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //Length
        {
            static_assert(Vec::Length() == 2);
        }

        //Component access
        {
            constexpr Vec v(T(10), T(5));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(5));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);
        }

        //operator[]
        {
            constexpr Vec v(T(10), T(5));

            static_assert(v[0] == T(10));
            static_assert(v[1] == T(5));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            constexpr Vec v(T(10), T(5));

            static_assert(v.begin() == &v.x());
            static_assert(v.cbegin() == &v.x());
            static_assert(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.y()) + 1));
            static_assert(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.y()) + 1));
            static_assert(v.end() == ((&v.y()) + 1));
            static_assert(v.cend() == ((&v.y()) + 1));
            static_assert(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            static_assert(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2AccessorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //Length
        {
            REQUIRE(Vec::Length() == 2);
        }

        //Component access
        {
            const Vec v(T(10), T(5));

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(5));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);

            Vec v2(T(10), T(5));

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
            //Check that v2.x() returns T&
            static_assert(std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>);
        }

        //operator[]
        {
            const Vec v(T(10), T(5));

            REQUIRE(v[0] == T(10));
            REQUIRE(v[1] == T(5));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);

            Vec v2(T(10), T(5));

            REQUIRE(v2[0] == T(10));
            REQUIRE(v2[1] == T(5));
            //Check that v2[0] returns T&
            static_assert(std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>);
        }

        //at()
        {
            const Vec v(T(10), T(5));

            REQUIRE(v.at(0) == T(10));
            REQUIRE(v.at(1) == T(5));
            //Check that v.at(0) returns const T&
            static_assert(std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>);

            Vec v2(T(10), T(5));

            REQUIRE(v2.at(0) == T(10));
            REQUIRE(v2.at(1) == T(5));
            //Check that v2.at(0) returns T&
            static_assert(std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            Vec v(T(10), T(5));

            REQUIRE(v.begin() == &v.x());
            REQUIRE(v.cbegin() == &v.x());
            REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.y()) + 1));
            REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.y()) + 1));
            REQUIRE(v.end() == ((&v.y()) + 1));
            REQUIRE(v.cend() == ((&v.y()) + 1));
            REQUIRE(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            REQUIRE(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));

            const Vec v2(T(10), T(5));

            REQUIRE(v2.begin() == &v2.x());
            REQUIRE(v2.cbegin() == &v2.x());
            REQUIRE(v2.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.y()) + 1));
            REQUIRE(v2.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.y()) + 1));
            REQUIRE(v2.end() == ((&v2.y()) + 1));
            REQUIRE(v2.cend() == ((&v2.y()) + 1));
            REQUIRE(v2.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
            REQUIRE(v2.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec2OperatorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //operator==
        {
            constexpr Vec v(T(10), T(5));
            constexpr Vec v2(T(5), T(10));
            constexpr Vec v3(T(10), T(5));

            static_assert(!(v == v2));
            static_assert(v == v3);
        }

        //operator!=
        {
            constexpr Vec v(T(10), T(5));
            constexpr Vec v2(T(5), T(10));
            constexpr Vec v3(T(10), T(5));

            static_assert(v != v2);
            static_assert(!(v != v3));
        }

        //Unary operator+
        {
            constexpr Vec v(T(10), T(5));
            constexpr Vec v2 = +v;

            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(5));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            constexpr Vec v(T(10), T(5));
            constexpr Vec v2 = -v;

            static_assert(v2.x() == T(-10));
            static_assert(v2.y() == T(-5));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5));

            constexpr Vec v1{~v};
            static_assert(v1.x() == T(~v.x()));
            static_assert(v1.y() == T(~v.y()));
        }

        //Binary operator+
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v + T(20)};
            static_assert(v1.x() == T(30));
            static_assert(v1.y() == T(25));

            constexpr Vec v2{T(20) + v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(25));

            constexpr Vec v3{v + v};
            static_assert(v3.x() == T(20));
            static_assert(v3.y() == T(10));
        }

        //Binary operator-
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v - T(5)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(0));

            constexpr Vec v2{T(20) - v};
            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(15));

            constexpr Vec v3{v - v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
        }

        //Binary operator*
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v * T(2)};
            static_assert(v1.x() == T(20));
            static_assert(v1.y() == T(10));

            constexpr Vec v2{T(2) * v};
            static_assert(v2.x() == T(20));
            static_assert(v2.y() == T(10));

            constexpr Vec v3{v * v};
            static_assert(v3.x() == T(100));
            static_assert(v3.y() == T(25));
        }

        //Binary operator/
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v / T(2)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(2.5));

            constexpr Vec v2{T(20) / v};
            static_assert(v2.x() == T(2));
            static_assert(v2.y() == T(4));

            constexpr Vec v3{v / v};
            static_assert(v3.x() == T(1));
            static_assert(v3.y() == T(1));
        }

        //Binary operator%
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v % T(2)};
            static_assert(v1.x() == T(0));
            static_assert(v1.y() == T(1));

            constexpr Vec v2{T(20) % v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));

            constexpr Vec v3{v % v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v & T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(0));

            constexpr Vec v2{T(20) & v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(4));

            constexpr Vec v3{v & v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v | T(2)};
            static_assert(v1.x() == T(10));
            static_assert(v1.y() == T(7));

            constexpr Vec v2{T(20) | v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(21));

            constexpr Vec v3{v | v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5));

            constexpr Vec v1{v ^ T(2)};
            static_assert(v1.x() == T(8));
            static_assert(v1.y() == T(7));

            constexpr Vec v2{T(20) ^ v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(17));

            constexpr Vec v3{v ^ v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(1), T(2));

            constexpr Vec v1{v << T(2)};
            static_assert(v1.x() == T(4));
            static_assert(v1.y() == T(8));

            constexpr Vec v2{T(2) << v};
            static_assert(v2.x() == T(4));
            static_assert(v2.y() == T(8));

            constexpr Vec v3{v << v};
            static_assert(v3.x() == T(2));
            static_assert(v3.y() == T(8));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5));

            constexpr Vec v1{v >> T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(1));

            constexpr Vec v2{T(2) >> v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));

            constexpr Vec v3{v >> v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
        }

        //Binary operator&&
        {
            constexpr TRAP::Math::tVec2<bool> v(true, true);
            constexpr TRAP::Math::tVec2<bool> v1(false, false);
            constexpr TRAP::Math::tVec2<bool> v2(true, false);
            constexpr TRAP::Math::tVec2<bool> v3(false, true);

            static_assert((v && v) == v);
            static_assert((v1 && v1) == v1);
            static_assert((v2 && v2) == v2);
            static_assert((v3 && v3) == v3);
        }

        //Binary operator||
        {
            constexpr TRAP::Math::tVec2<bool> v(true, true);
            constexpr TRAP::Math::tVec2<bool> v1(false, false);
            constexpr TRAP::Math::tVec2<bool> v2(true, false);
            constexpr TRAP::Math::tVec2<bool> v3(false, true);

            static_assert((v || v) == v);
            static_assert((v1 || v1) == v1);
            static_assert((v2 || v2) == v2);
            static_assert((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2OperatorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        //operator+=
        {
            Vec v(T(10), T(5));
            v += f64(9.0);

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
        }
        {
            Vec v(T(10), T(5));
            TRAP::Math::tVec2<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
        }

        //operator-=
        {
            Vec v(T(10), T(5));
            v -= f64(4.0);

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
        }
        {
            Vec v(T(10), T(5));
            TRAP::Math::tVec2<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
        }

        //operator*=
        {
            Vec v(T(10), T(5));
            v *= f64(2.0);

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
        }
        {
            Vec v(T(10), T(5));
            TRAP::Math::tVec2<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
        }

        //operator/=
        {
            Vec v(T(10), T(5));
            v /= f64(2.0);

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
        }
        {
            Vec v(T(10), T(5));
            TRAP::Math::tVec2<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
        }

        //operator++
        {
            Vec v(T(10), T(5));
            v++;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
        }
        {
            Vec v(T(10), T(5));
            ++v;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
        }

        //operator--
        {
            Vec v(T(10), T(5));
            v--;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
        }
        {
            Vec v(T(10), T(5));
            --v;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
        }

        //operator%=
        {
            Vec v(T(10), T(5));
            v %= f64(2.0);

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
        }
        {
            Vec v(T(10), T(5));
            TRAP::Math::tVec2<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
        }

        //operator&=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5));
                v &= u8(2);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
            }
            {
                Vec v(T(10), T(5));
                TRAP::Math::tVec2<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5));
                v |= u8(2);

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
            }
            {
                Vec v(T(10), T(5));
                TRAP::Math::tVec2<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5));
                v ^= u8(2.0);

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
            }
            {
                Vec v(T(10), T(5));
                TRAP::Math::tVec2<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5));
                v <<= u8(2.0);

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
            }
            {
                Vec v(T(10), T(5));
                TRAP::Math::tVec2<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5));
                v >>= u8(2.0);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
            }
            {
                Vec v(T(10), T(5));
                TRAP::Math::tVec2<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
            }
        }

        //operator==
        {
            Vec v(T(10), T(5));
            Vec v2(T(5), T(10));

            REQUIRE(!(v == v2));
            REQUIRE(v == v);
        }

        //operator!=
        {
            Vec v(T(10), T(5));
            Vec v2(T(5), T(10));

            REQUIRE(v != v2);
            REQUIRE(!(v != v));
        }

        //Unary operator+
        {
            Vec v(T(10), T(5));
            Vec v2 = +v;

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            Vec v(T(10), T(5));
            Vec v2 = -v;

            REQUIRE(v2.x() == T(-10));
            REQUIRE(v2.y() == T(-5));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5));

            Vec v1{~v};
            REQUIRE(v1.x() == T(~v.x()));
            REQUIRE(v1.y() == T(~v.y()));
        }

        //Binary operator+
        {
            Vec v(T(10), T(5));

            Vec v1{v + T(20)};
            REQUIRE(v1.x() == T(30));
            REQUIRE(v1.y() == T(25));

            Vec v2{T(20) + v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(25));

            Vec v3{v + v};
            REQUIRE(v3.x() == T(20));
            REQUIRE(v3.y() == T(10));
        }

        //Binary operator-
        {
            Vec v(T(10), T(5));

            Vec v1{v - T(5)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(0));

            Vec v2{T(20) - v};
            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(15));

            Vec v3{v - v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
        }

        //Binary operator*
        {
            Vec v(T(10), T(5));

            Vec v1{v * T(2)};
            REQUIRE(v1.x() == T(20));
            REQUIRE(v1.y() == T(10));

            Vec v2{T(2) * v};
            REQUIRE(v2.x() == T(20));
            REQUIRE(v2.y() == T(10));

            Vec v3{v * v};
            REQUIRE(v3.x() == T(100));
            REQUIRE(v3.y() == T(25));
        }

        //Binary operator/
        {
            Vec v(T(10), T(5));

            Vec v1{v / T(2)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(2.5));

            Vec v2{T(20) / v};
            REQUIRE(v2.x() == T(2));
            REQUIRE(v2.y() == T(4));

            Vec v3{v / v};
            REQUIRE(v3.x() == T(1));
            REQUIRE(v3.y() == T(1));
        }

        //Binary operator%
        {
            Vec v(T(10), T(5));

            Vec v1{v % T(2)};
            REQUIRE(v1.x() == T(0));
            REQUIRE(v1.y() == T(1));

            Vec v2{T(20) % v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));

            Vec v3{v % v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5));

            Vec v1{v & T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(0));

            Vec v2{T(20) & v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(4));

            Vec v3{v & v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5));

            Vec v1{v | T(2)};
            REQUIRE(v1.x() == T(10));
            REQUIRE(v1.y() == T(7));

            Vec v2{T(20) | v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(21));

            Vec v3{v | v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5));

            Vec v1{v ^ T(2)};
            REQUIRE(v1.x() == T(8));
            REQUIRE(v1.y() == T(7));

            Vec v2{T(20) ^ v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(17));

            Vec v3{v ^ v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            Vec v(T(1), T(2));

            Vec v1{v << T(2)};
            REQUIRE(v1.x() == T(4));
            REQUIRE(v1.y() == T(8));

            Vec v2{T(2) << v};
            REQUIRE(v2.x() == T(4));
            REQUIRE(v2.y() == T(8));

            Vec v3{v << v};
            REQUIRE(v3.x() == T(2));
            REQUIRE(v3.y() == T(8));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5));

            Vec v1{v >> T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(1));

            Vec v2{T(2) >> v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));

            Vec v3{v >> v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
        }

        //Binary operator&&
        {
            TRAP::Math::tVec2<bool> v(true, true);
            TRAP::Math::tVec2<bool> v1(false, false);
            TRAP::Math::tVec2<bool> v2(true, false);
            TRAP::Math::tVec2<bool> v3(false, true);

            REQUIRE((v && v) == v);
            REQUIRE((v1 && v1) == v1);
            REQUIRE((v2 && v2) == v2);
            REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            TRAP::Math::tVec2<bool> v(true, true);
            TRAP::Math::tVec2<bool> v1(false, false);
            TRAP::Math::tVec2<bool> v2(true, false);
            TRAP::Math::tVec2<bool> v3(false, true);

            REQUIRE((v || v) == v);
            REQUIRE((v1 || v1) == v1);
            REQUIRE((v2 || v2) == v2);
            REQUIRE((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2HashRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        usize hash = std::hash<Vec>()(Vec(T(5), T(10)));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2FormatRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        Vec v(T(5), T(10));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<T, f32>)
            REQUIRE(str == fmt::format("Vec2f({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, f64>)
            REQUIRE(str == fmt::format("Vec2d({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, bool>)
            REQUIRE(str == fmt::format("Vec2b({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, i8>)
            REQUIRE(str == fmt::format("Vec2i8({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, i16>)
            REQUIRE(str == fmt::format("Vec2i16({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, i32>)
            REQUIRE(str == fmt::format("Vec2i32({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, i64>)
            REQUIRE(str == fmt::format("Vec2i64({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, u8>)
            REQUIRE(str == fmt::format("Vec2u8({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, u16>)
            REQUIRE(str == fmt::format("Vec2u16({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, u32>)
            REQUIRE(str == fmt::format("Vec2u32({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<T, u64>)
            REQUIRE(str == fmt::format("Vec2u64({}, {})", v.x(), v.y()));
		else
			REQUIRE(str == "Unknown type");
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec2GetCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        constexpr Vec v(T(5), T(10));

        static_assert(std::get<0>(v) == T(5));
        static_assert(std::get<1>(std::move(v)) == T(10));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec2GetRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<2, T>;

        {
            const Vec v(T(5), T(10));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            const Vec v(T(5), T(10));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
        {
            Vec v(T(5), T(10));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            Vec v(T(5), T(10));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
    }
}

TEST_CASE("TRAP::Math::Vec2", "[math][vec][vec2]")
{
    SECTION("Typedefs")
    {
        RunVec2TypedefsCompileTimeTests<u8>();
        RunVec2TypedefsCompileTimeTests<u16>();
        RunVec2TypedefsCompileTimeTests<u32>();
        RunVec2TypedefsCompileTimeTests<u64>();
        RunVec2TypedefsCompileTimeTests<i8>();
        RunVec2TypedefsCompileTimeTests<i16>();
        RunVec2TypedefsCompileTimeTests<i32>();
        RunVec2TypedefsCompileTimeTests<i64>();
        RunVec2TypedefsCompileTimeTests<f32>();
        RunVec2TypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunVec2ConstructorsCompileTimeTests<u8>();
        RunVec2ConstructorsCompileTimeTests<u16>();
        RunVec2ConstructorsCompileTimeTests<u32>();
        RunVec2ConstructorsCompileTimeTests<u64>();
        RunVec2ConstructorsCompileTimeTests<i8>();
        RunVec2ConstructorsCompileTimeTests<i16>();
        RunVec2ConstructorsCompileTimeTests<i32>();
        RunVec2ConstructorsCompileTimeTests<i64>();
        RunVec2ConstructorsCompileTimeTests<f32>();
        RunVec2ConstructorsCompileTimeTests<f64>();

        RunVec2ConstructorsRunTimeTests<u8>();
        RunVec2ConstructorsRunTimeTests<u16>();
        RunVec2ConstructorsRunTimeTests<u32>();
        RunVec2ConstructorsRunTimeTests<u64>();
        RunVec2ConstructorsRunTimeTests<i8>();
        RunVec2ConstructorsRunTimeTests<i16>();
        RunVec2ConstructorsRunTimeTests<i32>();
        RunVec2ConstructorsRunTimeTests<i64>();
        RunVec2ConstructorsRunTimeTests<f32>();
        RunVec2ConstructorsRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunVec2AssignmentsRunTimeTests<u8>();
        RunVec2AssignmentsRunTimeTests<u16>();
        RunVec2AssignmentsRunTimeTests<u32>();
        RunVec2AssignmentsRunTimeTests<u64>();
        RunVec2AssignmentsRunTimeTests<i8>();
        RunVec2AssignmentsRunTimeTests<i16>();
        RunVec2AssignmentsRunTimeTests<i32>();
        RunVec2AssignmentsRunTimeTests<i64>();
        RunVec2AssignmentsRunTimeTests<f32>();
        RunVec2AssignmentsRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunVec2AccessorCompileTimeTests<u8>();
        RunVec2AccessorCompileTimeTests<u16>();
        RunVec2AccessorCompileTimeTests<u32>();
        RunVec2AccessorCompileTimeTests<u64>();
        RunVec2AccessorCompileTimeTests<i8>();
        RunVec2AccessorCompileTimeTests<i16>();
        RunVec2AccessorCompileTimeTests<i32>();
        RunVec2AccessorCompileTimeTests<i64>();
        RunVec2AccessorCompileTimeTests<f32>();
        RunVec2AccessorCompileTimeTests<f64>();

        RunVec2AccessorRunTimeTests<u8>();
        RunVec2AccessorRunTimeTests<u16>();
        RunVec2AccessorRunTimeTests<u32>();
        RunVec2AccessorRunTimeTests<u64>();
        RunVec2AccessorRunTimeTests<i8>();
        RunVec2AccessorRunTimeTests<i16>();
        RunVec2AccessorRunTimeTests<i32>();
        RunVec2AccessorRunTimeTests<i64>();
        RunVec2AccessorRunTimeTests<f32>();
        RunVec2AccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunVec2OperatorCompileTimeTests<u8>();
        RunVec2OperatorCompileTimeTests<u16>();
        RunVec2OperatorCompileTimeTests<u32>();
        RunVec2OperatorCompileTimeTests<u64>();
        RunVec2OperatorCompileTimeTests<i8>();
        RunVec2OperatorCompileTimeTests<i16>();
        RunVec2OperatorCompileTimeTests<i32>();
        RunVec2OperatorCompileTimeTests<i64>();
        RunVec2OperatorCompileTimeTests<f32>();
        RunVec2OperatorCompileTimeTests<f64>();

        RunVec2OperatorRunTimeTests<u8>();
        RunVec2OperatorRunTimeTests<u16>();
        RunVec2OperatorRunTimeTests<u32>();
        RunVec2OperatorRunTimeTests<u64>();
        RunVec2OperatorRunTimeTests<i8>();
        RunVec2OperatorRunTimeTests<i16>();
        RunVec2OperatorRunTimeTests<i32>();
        RunVec2OperatorRunTimeTests<i64>();
        RunVec2OperatorRunTimeTests<f32>();
        RunVec2OperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunVec2HashRunTimeTests<u8>();
        RunVec2HashRunTimeTests<u16>();
        RunVec2HashRunTimeTests<u32>();
        RunVec2HashRunTimeTests<u64>();
        RunVec2HashRunTimeTests<i8>();
        RunVec2HashRunTimeTests<i16>();
        RunVec2HashRunTimeTests<i32>();
        RunVec2HashRunTimeTests<i64>();
        RunVec2HashRunTimeTests<f32>();
        RunVec2HashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunVec2FormatRunTimeTests<u8>();
        RunVec2FormatRunTimeTests<u16>();
        RunVec2FormatRunTimeTests<u32>();
        RunVec2FormatRunTimeTests<u64>();
        RunVec2FormatRunTimeTests<i8>();
        RunVec2FormatRunTimeTests<i16>();
        RunVec2FormatRunTimeTests<i32>();
        RunVec2FormatRunTimeTests<i64>();
        RunVec2FormatRunTimeTests<f32>();
        RunVec2FormatRunTimeTests<f64>();
    }

    SECTION("std::get")
    {
        RunVec2GetCompileTimeTests<u8>();
        RunVec2GetCompileTimeTests<u16>();
        RunVec2GetCompileTimeTests<u32>();
        RunVec2GetCompileTimeTests<u64>();
        RunVec2GetCompileTimeTests<i8>();
        RunVec2GetCompileTimeTests<i16>();
        RunVec2GetCompileTimeTests<i32>();
        RunVec2GetCompileTimeTests<i64>();
        RunVec2GetCompileTimeTests<f32>();
        RunVec2GetCompileTimeTests<f64>();

        RunVec2GetRunTimeTests<u8>();
        RunVec2GetRunTimeTests<u16>();
        RunVec2GetRunTimeTests<u32>();
        RunVec2GetRunTimeTests<u64>();
        RunVec2GetRunTimeTests<i8>();
        RunVec2GetRunTimeTests<i16>();
        RunVec2GetRunTimeTests<i32>();
        RunVec2GetRunTimeTests<i64>();
        RunVec2GetRunTimeTests<f32>();
        RunVec2GetRunTimeTests<f64>();
    }
}
