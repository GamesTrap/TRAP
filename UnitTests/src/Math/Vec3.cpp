#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec3TypedefsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        static_assert(std::same_as<typename Vec::value_type, T>);
        static_assert(std::same_as<typename Vec::pointer, T*>);
        static_assert(std::same_as<typename Vec::const_pointer, const T*>);
        static_assert(std::same_as<typename Vec::reference, T&>);
        static_assert(std::same_as<typename Vec::const_reference, const T&>);
        static_assert(std::same_as<typename Vec::iterator, typename std::array<T, 3>::iterator>);
        static_assert(std::same_as<typename Vec::const_iterator, typename std::array<T, 3>::const_iterator>);
        static_assert(std::same_as<typename Vec::size_type, u32>);
        static_assert(std::same_as<typename Vec::difference_type, isize>);
        static_assert(std::same_as<typename Vec::reverse_iterator, typename std::array<T, 3>::reverse_iterator>);
        static_assert(std::same_as<typename Vec::const_reverse_iterator, typename std::array<T, 3>::const_reverse_iterator>);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec3ConstructorsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //Default constructor
        {
            constexpr Vec v{};

            static_assert(v.x() == T(0));
            static_assert(v.y() == T(0));
            static_assert(v.z() == T(0));
        }

        //Copy constructor
        {
            constexpr Vec v{T(2), T(5), T(8)};
            constexpr Vec vCopy(v);

            static_assert(vCopy.x() == T(2));
            static_assert(vCopy.y() == T(5));
            static_assert(vCopy.z() == T(8));
        }

        //Move constructor
        {
        }

        //Scalar constructor
        {
            constexpr Vec v(T(5));

            static_assert(v.x() == T(5));
            static_assert(v.y() == T(5));
            static_assert(v.z() == T(5));
        }

        //Value constructor
        {
            constexpr Vec v(T(5), T(10), T(15));

            static_assert(v.x() == T(5));
            static_assert(v.y() == T(10));
            static_assert(v.z() == T(15));
        }

        //Conversion constructor
        {
            constexpr Vec v(u64(100), u8(5), u16(8));

            static_assert(v.x() == T(100));
            static_assert(v.y() == T(5));
            static_assert(v.z() == T(8));
        }

        //Conversion Vec2 constructor
        {
            {
                constexpr Vec v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), f32(3.0f));

                static_assert(v.x() == T(10));
                static_assert(v.y() == T(127));
                static_assert(v.z() == T(3));
            }
            {
                constexpr Vec v(f32(3.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

                static_assert(v.x() == T(3));
                static_assert(v.y() == T(10));
                static_assert(v.z() == T(127));
            }
        }

        //Conversion Vec3 constructor
        {
            constexpr Vec v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
            static_assert(v.z() == T(80));
        }

        //Conversion Vec4 constructor
        {
            constexpr Vec v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
            static_assert(v.z() == T(80));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3ConstructorsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //Default constructor
        {
            Vec v{};

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(0));
            REQUIRE(v.z() == T(0));
        }

        //Copy constructor
        {
            Vec v{T(2), T(5), T(8)};
            Vec vCopy(v);

            REQUIRE(vCopy.x() == T(2));
            REQUIRE(vCopy.y() == T(5));
            REQUIRE(vCopy.z() == T(8));
        }

        //Move constructor
        {
            Vec v(T(5), T(10), T(15));
            Vec vMove(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
            REQUIRE(vMove.z() == T(15));
        }

        //Scalar constructor
        {
            Vec v(T(5));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(5));
        }

        //Value constructor
        {
            Vec v(T(5), T(10), T(15));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(15));
        }

        //Conversion constructor
        {
            Vec v(u64(100), u8(5), u16(99));

            REQUIRE(v.x() == T(100));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(99));
        }

        //Conversion Vec2 constructor
        {
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(in, u8(9));

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(127));
                REQUIRE(v.z() == T(9));
            }
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(f32(3.0f), in);

                REQUIRE(v.x() == T(3));
                REQUIRE(v.y() == T(10));
                REQUIRE(v.z() == T(127));
            }
        }

        //Conversion Vec3 constructor
        {
            TRAP::Math::tVec3<f64> in(f64(10.0), f64(127.0), f64(80.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
            REQUIRE(v.z() == T(80));
        }

        //Conversion Vec4 constructor
        {
            TRAP::Math::tVec4<f64> in(f64(10.0), f64(127.0), f64(80.0), f64(1.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
            REQUIRE(v.z() == T(80));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3AssignmentsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //Move assignment operator
        {
            Vec v(T(5), T(10), T(15));
            Vec vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
            REQUIRE(vMove.z() == T(15));
        }

        //Copy assignment operator
        {
            const Vec v(T(5), T(10), T(15));
            Vec vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == T(5));
            REQUIRE(vCopy.y() == T(10));
            REQUIRE(vCopy.z() == T(15));
        }

        //Conversion copy assignment operator
        {
            TRAP::Math::tVec3<f64> v(f64(5.0), f64(10.0), f64(9.0));
            Vec vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == T(5));
            REQUIRE(vConversion.y() == T(10));
            REQUIRE(vConversion.z() == T(9));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec3AccessorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //Length
        {
            static_assert(Vec::Length() == 3);
        }

        //Component access
        {
            constexpr Vec v(T(10), T(5), T(15));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(5));
            static_assert(v.z() == T(15));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);
        }

        //operator[]
        {
            constexpr Vec v(T(10), T(5), T(15));

            static_assert(v[0] == T(10));
            static_assert(v[1] == T(5));
            static_assert(v[2] == T(15));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            constexpr Vec v(T(10), T(5), T(15));

            static_assert(v.begin() == &v.x());
            static_assert(v.cbegin() == &v.x());
            static_assert(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.z()) + 1));
            static_assert(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.z()) + 1));
            static_assert(v.end() == ((&v.z()) + 1));
            static_assert(v.cend() == ((&v.z()) + 1));
            static_assert(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            static_assert(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3AccessorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //Length
        {
            REQUIRE(Vec::Length() == 3);
        }

        //Component access
        {
            const Vec v(T(10), T(5), T(15));

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(15));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);

            Vec v2(T(10), T(5), T(15));

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
            REQUIRE(v2.z() == T(15));
            //Check that v2.x() returns T&
            static_assert(std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>);
        }

        //operator[]
        {
            const Vec v(T(10), T(5), T(15));

            REQUIRE(v[0] == T(10));
            REQUIRE(v[1] == T(5));
            REQUIRE(v[2] == T(15));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);

            Vec v2(T(10), T(5), T(15));

            REQUIRE(v2[0] == T(10));
            REQUIRE(v2[1] == T(5));
            REQUIRE(v2[2] == T(15));
            //Check that v2[0] returns T&
            static_assert(std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>);
        }

        //at()
        {
            const Vec v(T(10), T(5), T(15));

            REQUIRE(v.at(0) == T(10));
            REQUIRE(v.at(1) == T(5));
            REQUIRE(v.at(2) == T(15));
            //Check that v.at(0) returns const T&
            static_assert(std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>);

            Vec v2(T(10), T(5), T(15));

            REQUIRE(v2.at(0) == T(10));
            REQUIRE(v2.at(1) == T(5));
            REQUIRE(v2.at(2) == T(15));
            //Check that v2.at(0) returns T&
            static_assert(std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            Vec v(T(10), T(5), T(15));

            REQUIRE(v.begin() == &v.x());
            REQUIRE(v.cbegin() == &v.x());
            REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.z()) + 1));
            REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.z()) + 1));
            REQUIRE(v.end() == ((&v.z()) + 1));
            REQUIRE(v.cend() == ((&v.z()) + 1));
            REQUIRE(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            REQUIRE(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));

            const Vec v2(T(10), T(5), T(15));

            REQUIRE(v2.begin() == &v2.x());
            REQUIRE(v2.cbegin() == &v2.x());
            REQUIRE(v2.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.z()) + 1));
            REQUIRE(v2.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.z()) + 1));
            REQUIRE(v2.end() == ((&v2.z()) + 1));
            REQUIRE(v2.cend() == ((&v2.z()) + 1));
            REQUIRE(v2.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
            REQUIRE(v2.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec3OperatorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //operator==
        {
            constexpr Vec v(T(10), T(5), T(15));
            constexpr Vec v2(T(15), T(5), T(10));
            constexpr Vec v3(T(10), T(5), T(15));

            static_assert(!(v == v2));
            static_assert(v == v3);
        }

        //operator!=
        {
            constexpr Vec v(T(10), T(5), T(15));
            constexpr Vec v2(T(15), T(5), T(10));
            constexpr Vec v3(T(10), T(5), T(15));

            static_assert(v != v2);
            static_assert(!(v != v3));
        }

        //Unary operator+
        {
            constexpr Vec v(T(10), T(5), T(15));
            constexpr Vec v2 = +v;

            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(5));
            static_assert(v2.z() == T(15));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            constexpr Vec v(T(10), T(5), T(15));
            constexpr Vec v2 = -v;

            static_assert(v2.x() == T(-10));
            static_assert(v2.y() == T(-5));
            static_assert(v2.z() == T(-15));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5), T(10));

            constexpr Vec v1{~v};
            static_assert(v1.x() == T(~v.x()));
            static_assert(v1.y() == T(~v.y()));
            static_assert(v1.z() == T(~v.z()));
        }

        //Binary operator+
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v + T(20)};
            static_assert(v1.x() == T(30));
            static_assert(v1.y() == T(25));
            static_assert(v1.z() == T(35));

            constexpr Vec v2{T(20) + v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(25));
            static_assert(v2.z() == T(35));

            constexpr Vec v3{v + v};
            static_assert(v3.x() == T(20));
            static_assert(v3.y() == T(10));
            static_assert(v3.z() == T(30));
        }

        //Binary operator-
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v - T(5)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(0));
            static_assert(v1.z() == T(10));

            constexpr Vec v2{T(20) - v};
            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(15));
            static_assert(v2.z() == T(5));

            constexpr Vec v3{v - v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
        }

        //Binary operator*
        {
            constexpr Vec v(T(10), T(5), T(2));

            constexpr Vec v1{v * T(2)};
            static_assert(v1.x() == T(20));
            static_assert(v1.y() == T(10));
            static_assert(v1.z() == T(4));

            constexpr Vec v2{T(2) * v};
            static_assert(v2.x() == T(20));
            static_assert(v2.y() == T(10));
            static_assert(v2.z() == T(4));

            constexpr Vec v3{v * v};
            static_assert(v3.x() == T(100));
            static_assert(v3.y() == T(25));
            static_assert(v3.z() == T(4));
        }

        //Binary operator/
        {
            constexpr Vec v(T(10), T(5), T(20));

            constexpr Vec v1{v / T(2)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(2.5));
            static_assert(v1.z() == T(10));

            constexpr Vec v2{T(20) / v};
            static_assert(v2.x() == T(2));
            static_assert(v2.y() == T(4));
            static_assert(v2.z() == T(1));

            constexpr Vec v3{v / v};
            static_assert(v3.x() == T(1));
            static_assert(v3.y() == T(1));
            static_assert(v3.z() == T(1));
        }

        //Binary operator%
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v % T(2)};
            static_assert(v1.x() == T(0));
            static_assert(v1.y() == T(1));
            static_assert(v1.z() == T(1));

            constexpr Vec v2{T(20) % v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));
            static_assert(v2.z() == T(5));

            constexpr Vec v3{v % v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v & T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(0));
            static_assert(v1.z() == T(2));

            constexpr Vec v2{T(20) & v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(4));
            static_assert(v2.z() == T(4));

            constexpr Vec v3{v & v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
            static_assert(v3.z() == T(15));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v | T(2)};
            static_assert(v1.x() == T(10));
            static_assert(v1.y() == T(7));
            static_assert(v1.z() == T(15));

            constexpr Vec v2{T(20) | v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(21));
            static_assert(v2.z() == T(31));

            constexpr Vec v3{v | v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
            static_assert(v3.z() == T(15));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15));

            constexpr Vec v1{v ^ T(2)};
            static_assert(v1.x() == T(8));
            static_assert(v1.y() == T(7));
            static_assert(v1.z() == T(13));

            constexpr Vec v2{T(20) ^ v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(17));
            static_assert(v2.z() == T(27));

            constexpr Vec v3{v ^ v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(1), T(2), T(3));

            constexpr Vec v1{v << T(2)};
            static_assert(v1.x() == T(4));
            static_assert(v1.y() == T(8));
            static_assert(v1.z() == T(12));

            constexpr Vec v2{T(2) << v};
            static_assert(v2.x() == T(4));
            static_assert(v2.y() == T(8));
            static_assert(v2.z() == T(16));

            constexpr Vec v3{v << v};
            static_assert(v3.x() == T(2));
            static_assert(v3.y() == T(8));
            static_assert(v3.z() == T(24));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5), T(15));

            constexpr Vec v1{v >> T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(1));
            static_assert(v1.z() == T(3));

            constexpr Vec v2{T(2) >> v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));
            static_assert(v2.z() == T(0));

            constexpr Vec v3{v >> v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
        }

        //Binary operator&&
        {
            constexpr TRAP::Math::tVec3<bool> v(true, true, true);
            constexpr TRAP::Math::tVec3<bool> v1(false, false, false);
            constexpr TRAP::Math::tVec3<bool> v2(true, false, true);
            constexpr TRAP::Math::tVec3<bool> v3(false, true, false);

            static_assert((v && v) == v);
            static_assert((v1 && v1) == v1);
            static_assert((v2 && v2) == v2);
            static_assert((v3 && v3) == v3);
        }

        //Binary operator||
        {
            constexpr TRAP::Math::tVec3<bool> v(true, true, true);
            constexpr TRAP::Math::tVec3<bool> v1(false, false, false);
            constexpr TRAP::Math::tVec3<bool> v2(true, false, true);
            constexpr TRAP::Math::tVec3<bool> v3(false, true, false);

            static_assert((v || v) == v);
            static_assert((v1 || v1) == v1);
            static_assert((v2 || v2) == v2);
            static_assert((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3OperatorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        //operator+=
        {
            Vec v(T(10), T(5), T(15));
            v += f64(9.0);

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
            REQUIRE(v.z() == T(24));
        }
        {
            Vec v(T(10), T(5), T(15));
            TRAP::Math::tVec3<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
            REQUIRE(v.z() == T(24));
        }

        //operator-=
        {
            Vec v(T(10), T(5), T(15));
            v -= f64(4.0);

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(11));
        }
        {
            Vec v(T(10), T(5), T(15));
            TRAP::Math::tVec3<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(11));
        }

        //operator*=
        {
            Vec v(T(10), T(5), T(15));
            v *= f64(2.0);

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(30));
        }
        {
            Vec v(T(10), T(5), T(15));
            TRAP::Math::tVec3<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(30));
        }

        //operator/=
        {
            Vec v(T(10), T(5), T(15));
            v /= f64(2.0);

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
            REQUIRE(v.z() == T(7.5));
        }
        {
            Vec v(T(10), T(5), T(15));
            TRAP::Math::tVec3<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
            REQUIRE(v.z() == T(7.5));
        }

        //operator++
        {
            Vec v(T(10), T(5), T(15));
            v++;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
            REQUIRE(v.z() == T(16));
        }
        {
            Vec v(T(10), T(5), T(15));
            ++v;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
            REQUIRE(v.z() == T(16));
        }

        //operator--
        {
            Vec v(T(10), T(5), T(15));
            v--;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
            REQUIRE(v.z() == T(14));
        }
        {
            Vec v(T(10), T(5), T(15));
            --v;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
            REQUIRE(v.z() == T(14));
        }

        //operator%=
        {
            Vec v(T(10), T(5), T(15));
            v %= f64(2.0);

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(1));
        }
        {
            Vec v(T(10), T(5), T(15));
            TRAP::Math::tVec3<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(1));
        }

        //operator&=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15));
                v &= u8(2);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
                REQUIRE(v.z() == T(2));
            }
            {
                Vec v(T(10), T(5), T(15));
                TRAP::Math::tVec3<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
                REQUIRE(v.z() == T(2));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15));
                v |= u8(2);

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(15));
            }
            {
                Vec v(T(10), T(5), T(15));
                TRAP::Math::tVec3<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(15));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15));
                v ^= u8(2.0);

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(13));
            }
            {
                Vec v(T(10), T(5), T(15));
                TRAP::Math::tVec3<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(13));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15));
                v <<= u8(2.0);

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
                REQUIRE(v.z() == T(60));
            }
            {
                Vec v(T(10), T(5), T(15));
                TRAP::Math::tVec3<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
                REQUIRE(v.z() == T(60));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15));
                v >>= u8(2.0);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
                REQUIRE(v.z() == T(3));
            }
            {
                Vec v(T(10), T(5), T(15));
                TRAP::Math::tVec3<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
                REQUIRE(v.z() == T(3));
            }
        }

        //operator==
        {
            Vec v(T(10), T(5), T(15));
            Vec v2(T(15), T(5), T(10));

            REQUIRE(!(v == v2));
            REQUIRE(v == v);
        }

        //operator!=
        {
            Vec v(T(10), T(5), T(15));
            Vec v2(T(15), T(5), T(10));

            REQUIRE(v != v2);
            REQUIRE(!(v != v));
        }

        //Unary operator+
        {
            Vec v(T(10), T(5), T(15));
            Vec v2 = +v;

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
            REQUIRE(v2.z() == T(15));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            Vec v(T(10), T(5), T(15));
            Vec v2 = -v;

            REQUIRE(v2.x() == T(-10));
            REQUIRE(v2.y() == T(-5));
            REQUIRE(v2.z() == T(-15));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5), T(15));

            Vec v1{~v};
            REQUIRE(v1.x() == T(~v.x()));
            REQUIRE(v1.y() == T(~v.y()));
            REQUIRE(v1.z() == T(~v.z()));
        }

        //Binary operator+
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v + T(20)};
            REQUIRE(v1.x() == T(30));
            REQUIRE(v1.y() == T(25));
            REQUIRE(v1.z() == T(35));

            Vec v2{T(20) + v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(25));
            REQUIRE(v2.z() == T(35));

            Vec v3{v + v};
            REQUIRE(v3.x() == T(20));
            REQUIRE(v3.y() == T(10));
            REQUIRE(v3.z() == T(30));
        }

        //Binary operator-
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v - T(5)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(0));
            REQUIRE(v1.z() == T(10));

            Vec v2{T(20) - v};
            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(15));
            REQUIRE(v2.z() == T(5));

            Vec v3{v - v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
        }

        //Binary operator*
        {
            Vec v(T(10), T(5), T(2));

            Vec v1{v * T(2)};
            REQUIRE(v1.x() == T(20));
            REQUIRE(v1.y() == T(10));
            REQUIRE(v1.z() == T(4));

            Vec v2{T(2) * v};
            REQUIRE(v2.x() == T(20));
            REQUIRE(v2.y() == T(10));
            REQUIRE(v2.z() == T(4));

            Vec v3{v * v};
            REQUIRE(v3.x() == T(100));
            REQUIRE(v3.y() == T(25));
            REQUIRE(v3.z() == T(4));
        }

        //Binary operator/
        {
            Vec v(T(10), T(5), T(20));

            Vec v1{v / T(2)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(2.5));
            REQUIRE(v1.z() == T(10));

            Vec v2{T(20) / v};
            REQUIRE(v2.x() == T(2));
            REQUIRE(v2.y() == T(4));
            REQUIRE(v2.z() == T(1));

            Vec v3{v / v};
            REQUIRE(v3.x() == T(1));
            REQUIRE(v3.y() == T(1));
            REQUIRE(v3.z() == T(1));
        }

        //Binary operator%
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v % T(2)};
            REQUIRE(v1.x() == T(0));
            REQUIRE(v1.y() == T(1));
            REQUIRE(v1.z() == T(1));

            Vec v2{T(20) % v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));
            REQUIRE(v2.z() == T(5));

            Vec v3{v % v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v & T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(0));
            REQUIRE(v1.z() == T(2));

            Vec v2{T(20) & v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(4));
            REQUIRE(v2.z() == T(4));

            Vec v3{v & v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
            REQUIRE(v3.z() == T(15));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v | T(2)};
            REQUIRE(v1.x() == T(10));
            REQUIRE(v1.y() == T(7));
            REQUIRE(v1.z() == T(15));

            Vec v2{T(20) | v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(21));
            REQUIRE(v2.z() == T(31));

            Vec v3{v | v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
            REQUIRE(v3.z() == T(15));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15));

            Vec v1{v ^ T(2)};
            REQUIRE(v1.x() == T(8));
            REQUIRE(v1.y() == T(7));
            REQUIRE(v1.z() == T(13));

            Vec v2{T(20) ^ v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(17));
            REQUIRE(v2.z() == T(27));

            Vec v3{v ^ v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            Vec v(T(1), T(2), T(3));

            Vec v1{v << T(2)};
            REQUIRE(v1.x() == T(4));
            REQUIRE(v1.y() == T(8));
            REQUIRE(v1.z() == T(12));

            Vec v2{T(2) << v};
            REQUIRE(v2.x() == T(4));
            REQUIRE(v2.y() == T(8));
            REQUIRE(v2.z() == T(16));

            Vec v3{v << v};
            REQUIRE(v3.x() == T(2));
            REQUIRE(v3.y() == T(8));
            REQUIRE(v3.z() == T(24));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5), T(15));

            Vec v1{v >> T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(1));
            REQUIRE(v1.z() == T(3));

            Vec v2{T(2) >> v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));
            REQUIRE(v2.z() == T(0));

            Vec v3{v >> v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
        }

        //Binary operator&&
        {
            TRAP::Math::tVec3<bool> v(true, true, true);
            TRAP::Math::tVec3<bool> v1(false, false, false);
            TRAP::Math::tVec3<bool> v2(true, false, true);
            TRAP::Math::tVec3<bool> v3(false, true, false);

            REQUIRE((v && v) == v);
            REQUIRE((v1 && v1) == v1);
            REQUIRE((v2 && v2) == v2);
            REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            TRAP::Math::tVec3<bool> v(true, true, true);
            TRAP::Math::tVec3<bool> v1(false, false, false);
            TRAP::Math::tVec3<bool> v2(true, false, true);
            TRAP::Math::tVec3<bool> v3(false, true, false);

            REQUIRE((v || v) == v);
            REQUIRE((v1 || v1) == v1);
            REQUIRE((v2 || v2) == v2);
            REQUIRE((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3HashRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        usize hash = std::hash<Vec>()(Vec(T(5), T(10), T(15)));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3FormatRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        Vec v(T(5), T(10), T(15));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<T, f32>)
            REQUIRE(str == fmt::format("Vec3f({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, f64>)
            REQUIRE(str == fmt::format("Vec3d({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, bool>)
            REQUIRE(str == fmt::format("Vec3b({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, i8>)
            REQUIRE(str == fmt::format("Vec3i8({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, i16>)
            REQUIRE(str == fmt::format("Vec3i16({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, i32>)
            REQUIRE(str == fmt::format("Vec3i32({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, i64>)
            REQUIRE(str == fmt::format("Vec3i64({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, u8>)
            REQUIRE(str == fmt::format("Vec3u8({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, u16>)
            REQUIRE(str == fmt::format("Vec3u16({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, u32>)
            REQUIRE(str == fmt::format("Vec3u32({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<T, u64>)
            REQUIRE(str == fmt::format("Vec3u64({}, {}, {})", v.x(), v.y(), v.z()));
		else
			REQUIRE(str == "Unknown type");
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec3GetCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        constexpr Vec v(T(5), T(10), T(15));

        static_assert(std::get<0>(v) == T(5));
        static_assert(std::get<1>(std::move(v)) == T(10));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec3GetRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<3, T>;

        {
            const Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<2>(v) == T(15));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            const Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
        {
            Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<2>(v) == T(15));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
        {
            Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<2>(std::move(v)) == T(15));
        }
        {
            const Vec v(T(5), T(10), T(15));

            REQUIRE(std::get<2>(std::move(v)) == T(15));
        }
    }
}

TEST_CASE("TRAP::Math::Vec3", "[math][vec][vec3]")
{
    SECTION("Typedefs")
    {
        RunVec3TypedefsCompileTimeTests<u8>();
        RunVec3TypedefsCompileTimeTests<u16>();
        RunVec3TypedefsCompileTimeTests<u32>();
        RunVec3TypedefsCompileTimeTests<u64>();
        RunVec3TypedefsCompileTimeTests<i8>();
        RunVec3TypedefsCompileTimeTests<i16>();
        RunVec3TypedefsCompileTimeTests<i32>();
        RunVec3TypedefsCompileTimeTests<i64>();
        RunVec3TypedefsCompileTimeTests<f32>();
        RunVec3TypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunVec3ConstructorsCompileTimeTests<u8>();
        RunVec3ConstructorsCompileTimeTests<u16>();
        RunVec3ConstructorsCompileTimeTests<u32>();
        RunVec3ConstructorsCompileTimeTests<u64>();
        RunVec3ConstructorsCompileTimeTests<i8>();
        RunVec3ConstructorsCompileTimeTests<i16>();
        RunVec3ConstructorsCompileTimeTests<i32>();
        RunVec3ConstructorsCompileTimeTests<i64>();
        RunVec3ConstructorsCompileTimeTests<f32>();
        RunVec3ConstructorsCompileTimeTests<f64>();

        RunVec3ConstructorsRunTimeTests<u8>();
        RunVec3ConstructorsRunTimeTests<u16>();
        RunVec3ConstructorsRunTimeTests<u32>();
        RunVec3ConstructorsRunTimeTests<u64>();
        RunVec3ConstructorsRunTimeTests<i8>();
        RunVec3ConstructorsRunTimeTests<i16>();
        RunVec3ConstructorsRunTimeTests<i32>();
        RunVec3ConstructorsRunTimeTests<i64>();
        RunVec3ConstructorsRunTimeTests<f32>();
        RunVec3ConstructorsRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunVec3AssignmentsRunTimeTests<u8>();
        RunVec3AssignmentsRunTimeTests<u16>();
        RunVec3AssignmentsRunTimeTests<u32>();
        RunVec3AssignmentsRunTimeTests<u64>();
        RunVec3AssignmentsRunTimeTests<i8>();
        RunVec3AssignmentsRunTimeTests<i16>();
        RunVec3AssignmentsRunTimeTests<i32>();
        RunVec3AssignmentsRunTimeTests<i64>();
        RunVec3AssignmentsRunTimeTests<f32>();
        RunVec3AssignmentsRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunVec3AccessorCompileTimeTests<u8>();
        RunVec3AccessorCompileTimeTests<u16>();
        RunVec3AccessorCompileTimeTests<u32>();
        RunVec3AccessorCompileTimeTests<u64>();
        RunVec3AccessorCompileTimeTests<i8>();
        RunVec3AccessorCompileTimeTests<i16>();
        RunVec3AccessorCompileTimeTests<i32>();
        RunVec3AccessorCompileTimeTests<i64>();
        RunVec3AccessorCompileTimeTests<f32>();
        RunVec3AccessorCompileTimeTests<f64>();

        RunVec3AccessorRunTimeTests<u8>();
        RunVec3AccessorRunTimeTests<u16>();
        RunVec3AccessorRunTimeTests<u32>();
        RunVec3AccessorRunTimeTests<u64>();
        RunVec3AccessorRunTimeTests<i8>();
        RunVec3AccessorRunTimeTests<i16>();
        RunVec3AccessorRunTimeTests<i32>();
        RunVec3AccessorRunTimeTests<i64>();
        RunVec3AccessorRunTimeTests<f32>();
        RunVec3AccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunVec3OperatorCompileTimeTests<u8>();
        RunVec3OperatorCompileTimeTests<u16>();
        RunVec3OperatorCompileTimeTests<u32>();
        RunVec3OperatorCompileTimeTests<u64>();
        RunVec3OperatorCompileTimeTests<i8>();
        RunVec3OperatorCompileTimeTests<i16>();
        RunVec3OperatorCompileTimeTests<i32>();
        RunVec3OperatorCompileTimeTests<i64>();
        RunVec3OperatorCompileTimeTests<f32>();
        RunVec3OperatorCompileTimeTests<f64>();

        RunVec3OperatorRunTimeTests<u8>();
        RunVec3OperatorRunTimeTests<u16>();
        RunVec3OperatorRunTimeTests<u32>();
        RunVec3OperatorRunTimeTests<u64>();
        RunVec3OperatorRunTimeTests<i8>();
        RunVec3OperatorRunTimeTests<i16>();
        RunVec3OperatorRunTimeTests<i32>();
        RunVec3OperatorRunTimeTests<i64>();
        RunVec3OperatorRunTimeTests<f32>();
        RunVec3OperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunVec3HashRunTimeTests<u8>();
        RunVec3HashRunTimeTests<u16>();
        RunVec3HashRunTimeTests<u32>();
        RunVec3HashRunTimeTests<u64>();
        RunVec3HashRunTimeTests<i8>();
        RunVec3HashRunTimeTests<i16>();
        RunVec3HashRunTimeTests<i32>();
        RunVec3HashRunTimeTests<i64>();
        RunVec3HashRunTimeTests<f32>();
        RunVec3HashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunVec3FormatRunTimeTests<u8>();
        RunVec3FormatRunTimeTests<u16>();
        RunVec3FormatRunTimeTests<u32>();
        RunVec3FormatRunTimeTests<u64>();
        RunVec3FormatRunTimeTests<i8>();
        RunVec3FormatRunTimeTests<i16>();
        RunVec3FormatRunTimeTests<i32>();
        RunVec3FormatRunTimeTests<i64>();
        RunVec3FormatRunTimeTests<f32>();
        RunVec3FormatRunTimeTests<f64>();
    }

    SECTION("std::get")
    {
        RunVec3GetCompileTimeTests<u8>();
        RunVec3GetCompileTimeTests<u16>();
        RunVec3GetCompileTimeTests<u32>();
        RunVec3GetCompileTimeTests<u64>();
        RunVec3GetCompileTimeTests<i8>();
        RunVec3GetCompileTimeTests<i16>();
        RunVec3GetCompileTimeTests<i32>();
        RunVec3GetCompileTimeTests<i64>();
        RunVec3GetCompileTimeTests<f32>();
        RunVec3GetCompileTimeTests<f64>();

        RunVec3GetRunTimeTests<u8>();
        RunVec3GetRunTimeTests<u16>();
        RunVec3GetRunTimeTests<u32>();
        RunVec3GetRunTimeTests<u64>();
        RunVec3GetRunTimeTests<i8>();
        RunVec3GetRunTimeTests<i16>();
        RunVec3GetRunTimeTests<i32>();
        RunVec3GetRunTimeTests<i64>();
        RunVec3GetRunTimeTests<f32>();
        RunVec3GetRunTimeTests<f64>();
    }
}
