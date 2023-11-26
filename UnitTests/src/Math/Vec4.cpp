#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec4TypedefsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        static_assert(std::same_as<typename Vec::value_type, T>);
        static_assert(std::same_as<typename Vec::pointer, T*>);
        static_assert(std::same_as<typename Vec::const_pointer, const T*>);
        static_assert(std::same_as<typename Vec::reference, T&>);
        static_assert(std::same_as<typename Vec::const_reference, const T&>);
        static_assert(std::same_as<typename Vec::iterator, typename std::array<T, 4>::iterator>);
        static_assert(std::same_as<typename Vec::const_iterator, typename std::array<T, 4>::const_iterator>);
        static_assert(std::same_as<typename Vec::size_type, u32>);
        static_assert(std::same_as<typename Vec::difference_type, isize>);
        static_assert(std::same_as<typename Vec::reverse_iterator, typename std::array<T, 4>::reverse_iterator>);
        static_assert(std::same_as<typename Vec::const_reverse_iterator, typename std::array<T, 4>::const_reverse_iterator>);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec4ConstructorsCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //Default constructor
        {
            constexpr Vec v{};

            static_assert(v.x() == T(0));
            static_assert(v.y() == T(0));
            static_assert(v.z() == T(0));
            static_assert(v.w() == T(0));
        }

        //Copy constructor
        {
            constexpr Vec v{T(2), T(5), T(8), T(10)};
            constexpr Vec vCopy(v);

            static_assert(vCopy.x() == T(2));
            static_assert(vCopy.y() == T(5));
            static_assert(vCopy.z() == T(8));
            static_assert(vCopy.w() == T(10));
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
            static_assert(v.w() == T(5));
        }

        //Value constructor
        {
            constexpr Vec v(T(5), T(10), T(15), T(20));

            static_assert(v.x() == T(5));
            static_assert(v.y() == T(10));
            static_assert(v.z() == T(15));
            static_assert(v.w() == T(20));
        }

        //Conversion constructor
        {
            constexpr Vec v(u64(100), u8(5), u16(8), f32(10.0f));

            static_assert(v.x() == T(100));
            static_assert(v.y() == T(5));
            static_assert(v.z() == T(8));
            static_assert(v.w() == T(10));
        }

        //Conversion Vec2 constructor
        {
            {
                constexpr Vec v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), f32(3.0f), f32(6.0f));

                static_assert(v.x() == T(10));
                static_assert(v.y() == T(127));
                static_assert(v.z() == T(3));
                static_assert(v.w() == T(6.0f));
            }
            {
                constexpr Vec v(f32(3.0f), f32(6.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

                static_assert(v.x() == T(3));
                static_assert(v.y() == T(6));
                static_assert(v.z() == T(10));
                static_assert(v.w() == T(127));
            }
            {
                constexpr Vec v(f32(3.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), u32(20));

                static_assert(v.x() == T(3));
                static_assert(v.y() == T(10));
                static_assert(v.z() == T(127));
                static_assert(v.w() == T(20));
            }
            {
                constexpr Vec v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), TRAP::Math::tVec2<f32>(f32(10.0), f32(127.0)));

                static_assert(v.x() == T(10));
                static_assert(v.y() == T(127));
                static_assert(v.z() == T(10));
                static_assert(v.w() == T(127));
            }
        }

        //Conversion Vec3 constructor
        {
            {
                constexpr Vec v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)), f32(5.0f));

                static_assert(v.x() == T(10));
                static_assert(v.y() == T(127));
                static_assert(v.z() == T(80));
                static_assert(v.w() == T(5.0f));
            }
            {
                constexpr Vec v(f32(5.0f), TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

                static_assert(v.x() == T(5.0f));
                static_assert(v.y() == T(10));
                static_assert(v.z() == T(127));
                static_assert(v.w() == T(80));
            }
        }

        //Conversion Vec4 constructor
        {
            constexpr Vec v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(127));
            static_assert(v.z() == T(80));
            static_assert(v.w() == T(1));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4ConstructorsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //Default constructor
        {
            Vec v{};

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(0));
            REQUIRE(v.z() == T(0));
            REQUIRE(v.w() == T(0));
        }

        //Copy constructor
        {
            Vec v{T(2), T(5), T(8), T(10)};
            Vec vCopy(v);

            REQUIRE(vCopy.x() == T(2));
            REQUIRE(vCopy.y() == T(5));
            REQUIRE(vCopy.z() == T(8));
            REQUIRE(vCopy.w() == T(10));
        }

        //Move constructor
        {
            Vec v(T(5), T(10), T(15), T(20));
            Vec vMove(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
            REQUIRE(vMove.z() == T(15));
            REQUIRE(vMove.w() == T(20));
        }

        //Scalar constructor
        {
            Vec v(T(5));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(5));
            REQUIRE(v.w() == T(5));
        }

        //Value constructor
        {
            Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(15));
            REQUIRE(v.w() == T(20));
        }

        //Conversion constructor
        {
            Vec v(u64(100), u8(5), u16(99), f32(10.0f));

            REQUIRE(v.x() == T(100));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(99));
            REQUIRE(v.w() == T(10));
        }

        //Conversion Vec2 constructor
        {
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(in, u8(9), u32(20));

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(127));
                REQUIRE(v.z() == T(9));
                REQUIRE(v.w() == T(20));
            }
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(f32(3.0f), u32(20), in);

                REQUIRE(v.x() == T(3));
                REQUIRE(v.y() == T(20));
                REQUIRE(v.z() == T(10));
                REQUIRE(v.w() == T(127));
            }
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(f32(3.0f), in, u32(20));

                REQUIRE(v.x() == T(3));
                REQUIRE(v.y() == T(10));
                REQUIRE(v.z() == T(127));
                REQUIRE(v.w() == T(20));
            }
            {
                TRAP::Math::tVec2<f64> in(f64(10.0), f64(127.0));
                Vec v(in, in);

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(127));
                REQUIRE(v.z() == T(10));
                REQUIRE(v.w() == T(127));
            }
        }

        //Conversion Vec3 constructor
        {
            {
                TRAP::Math::tVec3<f64> in(f64(10.0), f64(127.0), f64(80.0));
                Vec v(in, f32(5.0f));

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(127));
                REQUIRE(v.z() == T(80));
                REQUIRE(v.w() == T(5.0f));
            }
            {
                TRAP::Math::tVec3<f64> in(f64(10.0), f64(127.0), f64(80.0));
                Vec v(f32(5.0f), in);

                REQUIRE(v.x() == T(5.0f));
                REQUIRE(v.y() == T(10));
                REQUIRE(v.z() == T(127));
                REQUIRE(v.w() == T(80));
            }
        }

        //Conversion Vec4 constructor
        {
            TRAP::Math::tVec4<f64> in(f64(10.0), f64(127.0), f64(80.0), f64(1.0));
            Vec v(in);

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(127));
            REQUIRE(v.z() == T(80));
            REQUIRE(v.w() == T(1));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4AssignmentsRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //Move assignment operator
        {
            Vec v(T(5), T(10), T(15), T(20));
            Vec vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == T(5));
            REQUIRE(vMove.y() == T(10));
            REQUIRE(vMove.z() == T(15));
            REQUIRE(vMove.w() == T(20));
        }

        //Copy assignment operator
        {
            const Vec v(T(5), T(10), T(15), T(20));
            Vec vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == T(5));
            REQUIRE(vCopy.y() == T(10));
            REQUIRE(vCopy.z() == T(15));
            REQUIRE(vCopy.w() == T(20));
        }

        //Conversion copy assignment operator
        {
            TRAP::Math::tVec4<f64> v(f64(5.0), f64(10.0), f64(9.0), f64(20.0));
            Vec vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == T(5));
            REQUIRE(vConversion.y() == T(10));
            REQUIRE(vConversion.z() == T(9));
            REQUIRE(vConversion.w() == T(20));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec4AccessorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //Length
        {
            static_assert(Vec::Length() == 4);
        }

        //Component access
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            static_assert(v.x() == T(10));
            static_assert(v.y() == T(5));
            static_assert(v.z() == T(15));
            static_assert(v.w() == T(20));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);
        }

        //operator[]
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            static_assert(v[0] == T(10));
            static_assert(v[1] == T(5));
            static_assert(v[2] == T(15));
            static_assert(v[3] == T(20));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            static_assert(v.begin() == &v.x());
            static_assert(v.cbegin() == &v.x());
            static_assert(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.w()) + 1));
            static_assert(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.w()) + 1));
            static_assert(v.end() == ((&v.w()) + 1));
            static_assert(v.cend() == ((&v.w()) + 1));
            static_assert(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            static_assert(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4AccessorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //Length
        {
            REQUIRE(Vec::Length() == 4);
        }

        //Component access
        {
            const Vec v(T(10), T(5), T(15), T(20));

            REQUIRE(v.x() == T(10));
            REQUIRE(v.y() == T(5));
            REQUIRE(v.z() == T(15));
            REQUIRE(v.w() == T(20));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);

            Vec v2(T(10), T(5), T(15), T(20));

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
            REQUIRE(v2.z() == T(15));
            REQUIRE(v2.w() == T(20));
            //Check that v2.x() returns T&
            static_assert(std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>);
        }

        //operator[]
        {
            const Vec v(T(10), T(5), T(15), T(20));

            REQUIRE(v[0] == T(10));
            REQUIRE(v[1] == T(5));
            REQUIRE(v[2] == T(15));
            REQUIRE(v[3] == T(20));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);

            Vec v2(T(10), T(5), T(15), T(20));

            REQUIRE(v2[0] == T(10));
            REQUIRE(v2[1] == T(5));
            REQUIRE(v2[2] == T(15));
            REQUIRE(v2[3] == T(20));
            //Check that v2[0] returns T&
            static_assert(std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>);
        }

        //at()
        {
            const Vec v(T(10), T(5), T(15), T(20));

            REQUIRE(v.at(0) == T(10));
            REQUIRE(v.at(1) == T(5));
            REQUIRE(v.at(2) == T(15));
            REQUIRE(v.at(3) == T(20));
            //Check that v.at(0) returns const T&
            static_assert(std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>);

            Vec v2(T(10), T(5), T(15), T(20));

            REQUIRE(v2.at(0) == T(10));
            REQUIRE(v2.at(1) == T(5));
            REQUIRE(v2.at(2) == T(15));
            REQUIRE(v2.at(3) == T(20));
            //Check that v2.at(0) returns T&
            static_assert(std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            Vec v(T(10), T(5), T(15), T(20));

            REQUIRE(v.begin() == &v.x());
            REQUIRE(v.cbegin() == &v.x());
            REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.w()) + 1));
            REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v.w()) + 1));
            REQUIRE(v.end() == ((&v.w()) + 1));
            REQUIRE(v.cend() == ((&v.w()) + 1));
            REQUIRE(v.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));
            REQUIRE(v.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v.x()));

            const Vec v2(T(10), T(5), T(15), T(20));

            REQUIRE(v2.begin() == &v2.x());
            REQUIRE(v2.cbegin() == &v2.x());
            REQUIRE(v2.rbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.w()) + 1));
            REQUIRE(v2.crbegin() == std::reverse_iterator<typename Vec::const_pointer>((&v2.w()) + 1));
            REQUIRE(v2.end() == ((&v2.w()) + 1));
            REQUIRE(v2.cend() == ((&v2.w()) + 1));
            REQUIRE(v2.rend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
            REQUIRE(v2.crend() == std::reverse_iterator<typename Vec::const_pointer>(&v2.x()));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec4OperatorCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //operator==
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));
            constexpr Vec v2(T(20), T(15), T(5), T(10));
            constexpr Vec v3(T(10), T(5), T(15), T(20));

            static_assert(!(v == v2));
            static_assert(v == v3);
        }

        //operator!=
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));
            constexpr Vec v2(T(20), T(15), T(5), T(10));
            constexpr Vec v3(T(10), T(5), T(15), T(20));

            static_assert(v != v2);
            static_assert(!(v != v3));
        }

        //Unary operator+
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));
            constexpr Vec v2 = +v;

            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(5));
            static_assert(v2.z() == T(15));
            static_assert(v2.w() == T(20));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));
            constexpr Vec v2 = -v;

            static_assert(v2.x() == T(-10));
            static_assert(v2.y() == T(-5));
            static_assert(v2.z() == T(-15));
            static_assert(v2.w() == T(-20));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5), T(10), T(20));

            constexpr Vec v1{~v};
            static_assert(v1.x() == T(~v.x()));
            static_assert(v1.y() == T(~v.y()));
            static_assert(v1.z() == T(~v.z()));
            static_assert(v1.w() == T(~v.w()));
        }

        //Binary operator+
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v + T(20)};
            static_assert(v1.x() == T(30));
            static_assert(v1.y() == T(25));
            static_assert(v1.z() == T(35));
            static_assert(v1.w() == T(40));

            constexpr Vec v2{T(20) + v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(25));
            static_assert(v2.z() == T(35));
            static_assert(v2.w() == T(40));

            constexpr Vec v3{v + v};
            static_assert(v3.x() == T(20));
            static_assert(v3.y() == T(10));
            static_assert(v3.z() == T(30));
            static_assert(v3.w() == T(40));
        }

        //Binary operator-
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v - T(5)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(0));
            static_assert(v1.z() == T(10));
            static_assert(v1.w() == T(15));

            constexpr Vec v2{T(20) - v};
            static_assert(v2.x() == T(10));
            static_assert(v2.y() == T(15));
            static_assert(v2.z() == T(5));
            static_assert(v2.w() == T(0));

            constexpr Vec v3{v - v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
            static_assert(v3.w() == T(0));
        }

        //Binary operator*
        {
            constexpr Vec v(T(10), T(5), T(2), T(1));

            constexpr Vec v1{v * T(2)};
            static_assert(v1.x() == T(20));
            static_assert(v1.y() == T(10));
            static_assert(v1.z() == T(4));
            static_assert(v1.w() == T(2));

            constexpr Vec v2{T(2) * v};
            static_assert(v2.x() == T(20));
            static_assert(v2.y() == T(10));
            static_assert(v2.z() == T(4));
            static_assert(v2.w() == T(2));

            constexpr Vec v3{v * v};
            static_assert(v3.x() == T(100));
            static_assert(v3.y() == T(25));
            static_assert(v3.z() == T(4));
            static_assert(v3.w() == T(1));
        }

        //Binary operator/
        {
            constexpr Vec v(T(10), T(5), T(20), T(40));

            constexpr Vec v1{v / T(2)};
            static_assert(v1.x() == T(5));
            static_assert(v1.y() == T(2.5));
            static_assert(v1.z() == T(10));
            static_assert(v1.w() == T(20));

            constexpr Vec v2{T(20) / v};
            static_assert(v2.x() == T(2));
            static_assert(v2.y() == T(4));
            static_assert(v2.z() == T(1));
            static_assert(v2.w() == T(0.5));

            constexpr Vec v3{v / v};
            static_assert(v3.x() == T(1));
            static_assert(v3.y() == T(1));
            static_assert(v3.z() == T(1));
            static_assert(v3.w() == T(1));
        }

        //Binary operator%
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v % T(2)};
            static_assert(v1.x() == T(0));
            static_assert(v1.y() == T(1));
            static_assert(v1.z() == T(1));
            static_assert(v1.w() == T(0));

            constexpr Vec v2{T(20) % v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));
            static_assert(v2.z() == T(5));
            static_assert(v2.w() == T(0));

            constexpr Vec v3{v % v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
            static_assert(v3.w() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v & T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(0));
            static_assert(v1.z() == T(2));
            static_assert(v1.w() == T(0));

            constexpr Vec v2{T(20) & v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(4));
            static_assert(v2.z() == T(4));
            static_assert(v2.w() == T(20));

            constexpr Vec v3{v & v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
            static_assert(v3.z() == T(15));
            static_assert(v3.w() == T(20));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v | T(2)};
            static_assert(v1.x() == T(10));
            static_assert(v1.y() == T(7));
            static_assert(v1.z() == T(15));
            static_assert(v1.w() == T(22));

            constexpr Vec v2{T(20) | v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(21));
            static_assert(v2.z() == T(31));
            static_assert(v2.w() == T(20));

            constexpr Vec v3{v | v};
            static_assert(v3.x() == T(10));
            static_assert(v3.y() == T(5));
            static_assert(v3.z() == T(15));
            static_assert(v3.w() == T(20));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(10), T(5), T(15), T(20));

            constexpr Vec v1{v ^ T(2)};
            static_assert(v1.x() == T(8));
            static_assert(v1.y() == T(7));
            static_assert(v1.z() == T(13));
            static_assert(v1.w() == T(22));

            constexpr Vec v2{T(20) ^ v};
            static_assert(v2.x() == T(30));
            static_assert(v2.y() == T(17));
            static_assert(v2.z() == T(27));
            static_assert(v2.w() == T(0));

            constexpr Vec v3{v ^ v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
            static_assert(v3.w() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(1), T(2), T(3), T(4));

            constexpr Vec v1{v << T(2)};
            static_assert(v1.x() == T(4));
            static_assert(v1.y() == T(8));
            static_assert(v1.z() == T(12));
            static_assert(v1.w() == T(16));

            constexpr Vec v2{T(2) << v};
            static_assert(v2.x() == T(4));
            static_assert(v2.y() == T(8));
            static_assert(v2.z() == T(16));
            static_assert(v2.w() == T(32));

            constexpr Vec v3{v << v};
            static_assert(v3.x() == T(2));
            static_assert(v3.y() == T(8));
            static_assert(v3.z() == T(24));
            static_assert(v3.w() == T(64));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            constexpr Vec v(T(8), T(5), T(15), T(20));

            constexpr Vec v1{v >> T(2)};
            static_assert(v1.x() == T(2));
            static_assert(v1.y() == T(1));
            static_assert(v1.z() == T(3));
            static_assert(v1.w() == T(5));

            constexpr Vec v2{T(2) >> v};
            static_assert(v2.x() == T(0));
            static_assert(v2.y() == T(0));
            static_assert(v2.z() == T(0));
            static_assert(v2.w() == T(0));

            constexpr Vec v3{v >> v};
            static_assert(v3.x() == T(0));
            static_assert(v3.y() == T(0));
            static_assert(v3.z() == T(0));
            static_assert(v3.w() == T(0));
        }

        //Binary operator&&
        {
            constexpr TRAP::Math::tVec4<bool> v(true, true, true, true);
            constexpr TRAP::Math::tVec4<bool> v1(false, false, false, false);
            constexpr TRAP::Math::tVec4<bool> v2(true, false, true, false);
            constexpr TRAP::Math::tVec4<bool> v3(false, true, false, true);

            static_assert((v && v) == v);
            static_assert((v1 && v1) == v1);
            static_assert((v2 && v2) == v2);
            static_assert((v3 && v3) == v3);
        }

        //Binary operator||
        {
            constexpr TRAP::Math::tVec4<bool> v(true, true, true, true);
            constexpr TRAP::Math::tVec4<bool> v1(false, false, false, false);
            constexpr TRAP::Math::tVec4<bool> v2(true, false, true, false);
            constexpr TRAP::Math::tVec4<bool> v3(false, true, false, true);

            static_assert((v || v) == v);
            static_assert((v1 || v1) == v1);
            static_assert((v2 || v2) == v2);
            static_assert((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4OperatorRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        //operator+=
        {
            Vec v(T(10), T(5), T(15), T(20));
            v += f64(9.0);

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
            REQUIRE(v.z() == T(24));
            REQUIRE(v.w() == T(29));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            TRAP::Math::tVec4<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == T(19));
            REQUIRE(v.y() == T(14));
            REQUIRE(v.z() == T(24));
            REQUIRE(v.w() == T(29));
        }

        //operator-=
        {
            Vec v(T(10), T(5), T(15), T(20));
            v -= f64(4.0);

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(11));
            REQUIRE(v.w() == T(16));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            TRAP::Math::tVec4<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == T(6));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(11));
            REQUIRE(v.w() == T(16));
        }

        //operator*=
        {
            Vec v(T(10), T(5), T(15), T(20));
            v *= f64(2.0);

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(30));
            REQUIRE(v.w() == T(40));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            TRAP::Math::tVec4<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(10));
            REQUIRE(v.z() == T(30));
            REQUIRE(v.w() == T(40));
        }

        //operator/=
        {
            Vec v(T(10), T(5), T(15), T(20));
            v /= f64(2.0);

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
            REQUIRE(v.z() == T(7.5));
            REQUIRE(v.w() == T(10));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            TRAP::Math::tVec4<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == T(5));
            REQUIRE(v.y() == T(2.5));
            REQUIRE(v.z() == T(7.5));
            REQUIRE(v.w() == T(10));
        }

        //operator++
        {
            Vec v(T(10), T(5), T(15), T(20));
            v++;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
            REQUIRE(v.z() == T(16));
            REQUIRE(v.w() == T(21));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            ++v;

            REQUIRE(v.x() == T(11));
            REQUIRE(v.y() == T(6));
            REQUIRE(v.z() == T(16));
            REQUIRE(v.w() == T(21));
        }

        //operator--
        {
            Vec v(T(10), T(5), T(15), T(20));
            v--;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
            REQUIRE(v.z() == T(14));
            REQUIRE(v.w() == T(19));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            --v;

            REQUIRE(v.x() == T(9));
            REQUIRE(v.y() == T(4));
            REQUIRE(v.z() == T(14));
            REQUIRE(v.w() == T(19));
        }

        //operator%=
        {
            Vec v(T(10), T(5), T(15), T(20));
            v %= f64(2.0);

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(1));
            REQUIRE(v.w() == T(0));
        }
        {
            Vec v(T(10), T(5), T(15), T(20));
            TRAP::Math::tVec4<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == T(0));
            REQUIRE(v.y() == T(1));
            REQUIRE(v.z() == T(1));
            REQUIRE(v.w() == T(0));
        }

        //operator&=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15), T(20));
                v &= u8(2);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
                REQUIRE(v.z() == T(2));
                REQUIRE(v.w() == T(0));
            }
            {
                Vec v(T(10), T(5), T(15), T(20));
                TRAP::Math::tVec4<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(0));
                REQUIRE(v.z() == T(2));
                REQUIRE(v.w() == T(0));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15), T(20));
                v |= u8(2);

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(15));
                REQUIRE(v.w() == T(22));
            }
            {
                Vec v(T(10), T(5), T(15), T(20));
                TRAP::Math::tVec4<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == T(10));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(15));
                REQUIRE(v.w() == T(22));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15), T(20));
                v ^= u8(2.0);

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(13));
                REQUIRE(v.w() == T(22));
            }
            {
                Vec v(T(10), T(5), T(15), T(20));
                TRAP::Math::tVec4<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == T(8));
                REQUIRE(v.y() == T(7));
                REQUIRE(v.z() == T(13));
                REQUIRE(v.w() == T(22));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15), T(20));
                v <<= u8(2.0);

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
                REQUIRE(v.z() == T(60));
                REQUIRE(v.w() == T(80));
            }
            {
                Vec v(T(10), T(5), T(15), T(20));
                TRAP::Math::tVec4<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == T(40));
                REQUIRE(v.y() == T(20));
                REQUIRE(v.z() == T(60));
                REQUIRE(v.w() == T(80));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<T>)
        {
            {
                Vec v(T(10), T(5), T(15), T(20));
                v >>= u8(2.0);

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
                REQUIRE(v.z() == T(3));
                REQUIRE(v.w() == T(5));
            }
            {
                Vec v(T(10), T(5), T(15), T(20));
                TRAP::Math::tVec4<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == T(2));
                REQUIRE(v.y() == T(1));
                REQUIRE(v.z() == T(3));
                REQUIRE(v.w() == T(5));
            }
        }

        //operator==
        {
            Vec v(T(10), T(5), T(15), T(20));
            Vec v2(T(20), T(15), T(5), T(10));

            REQUIRE(!(v == v2));
            REQUIRE(v == v);
        }

        //operator!=
        {
            Vec v(T(10), T(5), T(15), T(20));
            Vec v2(T(20), T(15), T(5), T(10));

            REQUIRE(v != v2);
            REQUIRE(!(v != v));
        }

        //Unary operator+
        {
            Vec v(T(10), T(5), T(15), T(20));
            Vec v2 = +v;

            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(5));
            REQUIRE(v2.z() == T(15));
            REQUIRE(v2.w() == T(20));
        }

        //Unary operator-
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            Vec v(T(10), T(5), T(15), T(20));
            Vec v2 = -v;

            REQUIRE(v2.x() == T(-10));
            REQUIRE(v2.y() == T(-5));
            REQUIRE(v2.z() == T(-15));
            REQUIRE(v2.w() == T(-20));
        }

        //Unary operator~
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5), T(15), T(20));

            Vec v1{~v};
            REQUIRE(v1.x() == T(~v.x()));
            REQUIRE(v1.y() == T(~v.y()));
            REQUIRE(v1.z() == T(~v.z()));
            REQUIRE(v1.w() == T(~v.w()));
        }

        //Binary operator+
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v + T(20)};
            REQUIRE(v1.x() == T(30));
            REQUIRE(v1.y() == T(25));
            REQUIRE(v1.z() == T(35));
            REQUIRE(v1.w() == T(40));

            Vec v2{T(20) + v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(25));
            REQUIRE(v2.z() == T(35));
            REQUIRE(v2.w() == T(40));

            Vec v3{v + v};
            REQUIRE(v3.x() == T(20));
            REQUIRE(v3.y() == T(10));
            REQUIRE(v3.z() == T(30));
            REQUIRE(v3.w() == T(40));
        }

        //Binary operator-
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v - T(5)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(0));
            REQUIRE(v1.z() == T(10));
            REQUIRE(v1.w() == T(15));

            Vec v2{T(20) - v};
            REQUIRE(v2.x() == T(10));
            REQUIRE(v2.y() == T(15));
            REQUIRE(v2.z() == T(5));
            REQUIRE(v2.w() == T(0));

            Vec v3{v - v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
            REQUIRE(v3.w() == T(0));
        }

        //Binary operator*
        {
            Vec v(T(10), T(5), T(2), T(3));

            Vec v1{v * T(2)};
            REQUIRE(v1.x() == T(20));
            REQUIRE(v1.y() == T(10));
            REQUIRE(v1.z() == T(4));
            REQUIRE(v1.w() == T(6));

            Vec v2{T(2) * v};
            REQUIRE(v2.x() == T(20));
            REQUIRE(v2.y() == T(10));
            REQUIRE(v2.z() == T(4));
            REQUIRE(v2.w() == T(6));

            Vec v3{v * v};
            REQUIRE(v3.x() == T(100));
            REQUIRE(v3.y() == T(25));
            REQUIRE(v3.z() == T(4));
            REQUIRE(v3.w() == T(9));
        }

        //Binary operator/
        {
            Vec v(T(10), T(5), T(20), T(40));

            Vec v1{v / T(2)};
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(2.5));
            REQUIRE(v1.z() == T(10));
            REQUIRE(v1.w() == T(20));

            Vec v2{T(20) / v};
            REQUIRE(v2.x() == T(2));
            REQUIRE(v2.y() == T(4));
            REQUIRE(v2.z() == T(1));
            REQUIRE(v2.w() == T(0.5));

            Vec v3{v / v};
            REQUIRE(v3.x() == T(1));
            REQUIRE(v3.y() == T(1));
            REQUIRE(v3.z() == T(1));
            REQUIRE(v3.w() == T(1));
        }

        //Binary operator%
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v % T(2)};
            REQUIRE(v1.x() == T(0));
            REQUIRE(v1.y() == T(1));
            REQUIRE(v1.z() == T(1));
            REQUIRE(v1.w() == T(0));

            Vec v2{T(20) % v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));
            REQUIRE(v2.z() == T(5));
            REQUIRE(v2.w() == T(0));

            Vec v3{v % v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
            REQUIRE(v3.w() == T(0));
        }

        //Binary operator&
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v & T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(0));
            REQUIRE(v1.z() == T(2));
            REQUIRE(v1.w() == T(0));

            Vec v2{T(20) & v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(4));
            REQUIRE(v2.z() == T(4));
            REQUIRE(v2.w() == T(20));

            Vec v3{v & v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
            REQUIRE(v3.z() == T(15));
            REQUIRE(v3.w() == T(20));
        }

        //Binary operator|
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v | T(2)};
            REQUIRE(v1.x() == T(10));
            REQUIRE(v1.y() == T(7));
            REQUIRE(v1.z() == T(15));
            REQUIRE(v1.w() == T(22));

            Vec v2{T(20) | v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(21));
            REQUIRE(v2.z() == T(31));
            REQUIRE(v2.w() == T(20));

            Vec v3{v | v};
            REQUIRE(v3.x() == T(10));
            REQUIRE(v3.y() == T(5));
            REQUIRE(v3.z() == T(15));
            REQUIRE(v3.w() == T(20));
        }

        //Binary operator^
        if constexpr(std::integral<T>)
        {
            Vec v(T(10), T(5), T(15), T(20));

            Vec v1{v ^ T(2)};
            REQUIRE(v1.x() == T(8));
            REQUIRE(v1.y() == T(7));
            REQUIRE(v1.z() == T(13));
            REQUIRE(v1.w() == T(22));

            Vec v2{T(20) ^ v};
            REQUIRE(v2.x() == T(30));
            REQUIRE(v2.y() == T(17));
            REQUIRE(v2.z() == T(27));
            REQUIRE(v2.w() == T(0));

            Vec v3{v ^ v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
            REQUIRE(v3.w() == T(0));
        }

        //Binary operator<<
        if constexpr(std::integral<T>)
        {
            Vec v(T(1), T(2), T(3), T(4));

            Vec v1{v << T(2)};
            REQUIRE(v1.x() == T(4));
            REQUIRE(v1.y() == T(8));
            REQUIRE(v1.z() == T(12));
            REQUIRE(v1.w() == T(16));

            Vec v2{T(2) << v};
            REQUIRE(v2.x() == T(4));
            REQUIRE(v2.y() == T(8));
            REQUIRE(v2.z() == T(16));
            REQUIRE(v2.w() == T(32));

            Vec v3{v << v};
            REQUIRE(v3.x() == T(2));
            REQUIRE(v3.y() == T(8));
            REQUIRE(v3.z() == T(24));
            REQUIRE(v3.w() == T(64));
        }

        //Binary operator>>
        if constexpr(std::integral<T>)
        {
            Vec v(T(8), T(5), T(15), T(20));

            Vec v1{v >> T(2)};
            REQUIRE(v1.x() == T(2));
            REQUIRE(v1.y() == T(1));
            REQUIRE(v1.z() == T(3));
            REQUIRE(v1.w() == T(5));

            Vec v2{T(2) >> v};
            REQUIRE(v2.x() == T(0));
            REQUIRE(v2.y() == T(0));
            REQUIRE(v2.z() == T(0));
            REQUIRE(v2.w() == T(0));

            Vec v3{v >> v};
            REQUIRE(v3.x() == T(0));
            REQUIRE(v3.y() == T(0));
            REQUIRE(v3.z() == T(0));
            REQUIRE(v3.w() == T(0));
        }

        //Binary operator&&
        {
            TRAP::Math::tVec4<bool> v(true, true, true, true);
            TRAP::Math::tVec4<bool> v1(false, false, false, false);
            TRAP::Math::tVec4<bool> v2(true, false, true, false);
            TRAP::Math::tVec4<bool> v3(false, true, false, true);

            REQUIRE((v && v) == v);
            REQUIRE((v1 && v1) == v1);
            REQUIRE((v2 && v2) == v2);
            REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            TRAP::Math::tVec4<bool> v(true, true, true, true);
            TRAP::Math::tVec4<bool> v1(false, false, false, false);
            TRAP::Math::tVec4<bool> v2(true, false, true, false);
            TRAP::Math::tVec4<bool> v3(false, true, false, true);

            REQUIRE((v || v) == v);
            REQUIRE((v1 || v1) == v1);
            REQUIRE((v2 || v2) == v2);
            REQUIRE((v3 || v3) == v3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4HashRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        usize hash = std::hash<Vec>()(Vec(T(5), T(10), T(15), T(20)));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4FormatRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        Vec v(T(5), T(10), T(15), T(20));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<T, f32>)
            REQUIRE(str == fmt::format("Vec4f({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, f64>)
            REQUIRE(str == fmt::format("Vec4d({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, bool>)
            REQUIRE(str == fmt::format("Vec4b({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, i8>)
            REQUIRE(str == fmt::format("Vec4i8({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, i16>)
            REQUIRE(str == fmt::format("Vec4i16({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, i32>)
            REQUIRE(str == fmt::format("Vec4i32({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, i64>)
            REQUIRE(str == fmt::format("Vec4i64({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, u8>)
            REQUIRE(str == fmt::format("Vec4u8({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, u16>)
            REQUIRE(str == fmt::format("Vec4u16({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, u32>)
            REQUIRE(str == fmt::format("Vec4u32({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<T, u64>)
            REQUIRE(str == fmt::format("Vec4u64({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else
			REQUIRE(str == "Unknown type");
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunVec4GetCompileTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        constexpr Vec v(T(5), T(10), T(15), T(20));

        static_assert(std::get<0>(v) == T(5));
        static_assert(std::get<1>(std::move(v)) == T(10));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4GetRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        {
            const Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<2>(v) == T(15));
            REQUIRE(std::get<3>(v) == T(20));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            const Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
        {
            Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<0>(v) == T(5));
            REQUIRE(std::get<1>(v) == T(10));
            REQUIRE(std::get<2>(v) == T(15));
            REQUIRE(std::get<3>(v) == T(20));
            REQUIRE(std::get<1>(std::move(v)) == T(10));
        }
        {
            Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<0>(std::move(v)) == T(5));
        }
        {
            Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<2>(std::move(v)) == T(15));
        }
        {
            const Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<2>(std::move(v)) == T(15));
        }
        {
            Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<3>(std::move(v)) == T(20));
        }
        {
            const Vec v(T(5), T(10), T(15), T(20));

            REQUIRE(std::get<3>(std::move(v)) == T(20));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunVec4SwapRunTimeTests()
    {
        using Vec = TRAP::Math::Vec<4, T>;

        {
            Vec v(T(5), T(10), T(15), T(20));
            Vec v1(T(20), T(15), T(10), T(5));

            v.Swap(v1);

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(15));
            REQUIRE(v.z() == T(10));
            REQUIRE(v.w() == T(5));
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(10));
            REQUIRE(v1.z() == T(15));
            REQUIRE(v1.w() == T(20));
        }
        {
            Vec v(T(5), T(10), T(15), T(20));
            Vec v1(T(20), T(15), T(10), T(5));

            std::swap(v, v1);

            REQUIRE(v.x() == T(20));
            REQUIRE(v.y() == T(15));
            REQUIRE(v.z() == T(10));
            REQUIRE(v.w() == T(5));
            REQUIRE(v1.x() == T(5));
            REQUIRE(v1.y() == T(10));
            REQUIRE(v1.z() == T(15));
            REQUIRE(v1.w() == T(20));
        }
    }
}

TEST_CASE("TRAP::Math::Vec4", "[math][vec][vec4]")
{
    SECTION("Typedefs")
    {
        RunVec4TypedefsCompileTimeTests<u8>();
        RunVec4TypedefsCompileTimeTests<u16>();
        RunVec4TypedefsCompileTimeTests<u32>();
        RunVec4TypedefsCompileTimeTests<u64>();
        RunVec4TypedefsCompileTimeTests<i8>();
        RunVec4TypedefsCompileTimeTests<i16>();
        RunVec4TypedefsCompileTimeTests<i32>();
        RunVec4TypedefsCompileTimeTests<i64>();
        RunVec4TypedefsCompileTimeTests<f32>();
        RunVec4TypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunVec4ConstructorsCompileTimeTests<u8>();
        RunVec4ConstructorsCompileTimeTests<u16>();
        RunVec4ConstructorsCompileTimeTests<u32>();
        RunVec4ConstructorsCompileTimeTests<u64>();
        RunVec4ConstructorsCompileTimeTests<i8>();
        RunVec4ConstructorsCompileTimeTests<i16>();
        RunVec4ConstructorsCompileTimeTests<i32>();
        RunVec4ConstructorsCompileTimeTests<i64>();
        RunVec4ConstructorsCompileTimeTests<f32>();
        RunVec4ConstructorsCompileTimeTests<f64>();

        RunVec4ConstructorsRunTimeTests<u8>();
        RunVec4ConstructorsRunTimeTests<u16>();
        RunVec4ConstructorsRunTimeTests<u32>();
        RunVec4ConstructorsRunTimeTests<u64>();
        RunVec4ConstructorsRunTimeTests<i8>();
        RunVec4ConstructorsRunTimeTests<i16>();
        RunVec4ConstructorsRunTimeTests<i32>();
        RunVec4ConstructorsRunTimeTests<i64>();
        RunVec4ConstructorsRunTimeTests<f32>();
        RunVec4ConstructorsRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunVec4AssignmentsRunTimeTests<u8>();
        RunVec4AssignmentsRunTimeTests<u16>();
        RunVec4AssignmentsRunTimeTests<u32>();
        RunVec4AssignmentsRunTimeTests<u64>();
        RunVec4AssignmentsRunTimeTests<i8>();
        RunVec4AssignmentsRunTimeTests<i16>();
        RunVec4AssignmentsRunTimeTests<i32>();
        RunVec4AssignmentsRunTimeTests<i64>();
        RunVec4AssignmentsRunTimeTests<f32>();
        RunVec4AssignmentsRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunVec4AccessorCompileTimeTests<u8>();
        RunVec4AccessorCompileTimeTests<u16>();
        RunVec4AccessorCompileTimeTests<u32>();
        RunVec4AccessorCompileTimeTests<u64>();
        RunVec4AccessorCompileTimeTests<i8>();
        RunVec4AccessorCompileTimeTests<i16>();
        RunVec4AccessorCompileTimeTests<i32>();
        RunVec4AccessorCompileTimeTests<i64>();
        RunVec4AccessorCompileTimeTests<f32>();
        RunVec4AccessorCompileTimeTests<f64>();

        RunVec4AccessorRunTimeTests<u8>();
        RunVec4AccessorRunTimeTests<u16>();
        RunVec4AccessorRunTimeTests<u32>();
        RunVec4AccessorRunTimeTests<u64>();
        RunVec4AccessorRunTimeTests<i8>();
        RunVec4AccessorRunTimeTests<i16>();
        RunVec4AccessorRunTimeTests<i32>();
        RunVec4AccessorRunTimeTests<i64>();
        RunVec4AccessorRunTimeTests<f32>();
        RunVec4AccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunVec4OperatorCompileTimeTests<u8>();
        RunVec4OperatorCompileTimeTests<u16>();
        RunVec4OperatorCompileTimeTests<u32>();
        RunVec4OperatorCompileTimeTests<u64>();
        RunVec4OperatorCompileTimeTests<i8>();
        RunVec4OperatorCompileTimeTests<i16>();
        RunVec4OperatorCompileTimeTests<i32>();
        RunVec4OperatorCompileTimeTests<i64>();
        RunVec4OperatorCompileTimeTests<f32>();
        RunVec4OperatorCompileTimeTests<f64>();

        RunVec4OperatorRunTimeTests<u8>();
        RunVec4OperatorRunTimeTests<u16>();
        RunVec4OperatorRunTimeTests<u32>();
        RunVec4OperatorRunTimeTests<u64>();
        RunVec4OperatorRunTimeTests<i8>();
        RunVec4OperatorRunTimeTests<i16>();
        RunVec4OperatorRunTimeTests<i32>();
        RunVec4OperatorRunTimeTests<i64>();
        RunVec4OperatorRunTimeTests<f32>();
        RunVec4OperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunVec4HashRunTimeTests<u8>();
        RunVec4HashRunTimeTests<u16>();
        RunVec4HashRunTimeTests<u32>();
        RunVec4HashRunTimeTests<u64>();
        RunVec4HashRunTimeTests<i8>();
        RunVec4HashRunTimeTests<i16>();
        RunVec4HashRunTimeTests<i32>();
        RunVec4HashRunTimeTests<i64>();
        RunVec4HashRunTimeTests<f32>();
        RunVec4HashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunVec4FormatRunTimeTests<u8>();
        RunVec4FormatRunTimeTests<u16>();
        RunVec4FormatRunTimeTests<u32>();
        RunVec4FormatRunTimeTests<u64>();
        RunVec4FormatRunTimeTests<i8>();
        RunVec4FormatRunTimeTests<i16>();
        RunVec4FormatRunTimeTests<i32>();
        RunVec4FormatRunTimeTests<i64>();
        RunVec4FormatRunTimeTests<f32>();
        RunVec4FormatRunTimeTests<f64>();
    }

    SECTION("std::get")
    {
        RunVec4GetCompileTimeTests<u8>();
        RunVec4GetCompileTimeTests<u16>();
        RunVec4GetCompileTimeTests<u32>();
        RunVec4GetCompileTimeTests<u64>();
        RunVec4GetCompileTimeTests<i8>();
        RunVec4GetCompileTimeTests<i16>();
        RunVec4GetCompileTimeTests<i32>();
        RunVec4GetCompileTimeTests<i64>();
        RunVec4GetCompileTimeTests<f32>();
        RunVec4GetCompileTimeTests<f64>();

        RunVec4GetRunTimeTests<u8>();
        RunVec4GetRunTimeTests<u16>();
        RunVec4GetRunTimeTests<u32>();
        RunVec4GetRunTimeTests<u64>();
        RunVec4GetRunTimeTests<i8>();
        RunVec4GetRunTimeTests<i16>();
        RunVec4GetRunTimeTests<i32>();
        RunVec4GetRunTimeTests<i64>();
        RunVec4GetRunTimeTests<f32>();
        RunVec4GetRunTimeTests<f64>();
    }

    SECTION("std::swap")
    {
        RunVec4SwapRunTimeTests<u8>();
        RunVec4SwapRunTimeTests<u16>();
        RunVec4SwapRunTimeTests<u32>();
        RunVec4SwapRunTimeTests<u64>();
        RunVec4SwapRunTimeTests<i8>();
        RunVec4SwapRunTimeTests<i16>();
        RunVec4SwapRunTimeTests<i32>();
        RunVec4SwapRunTimeTests<i64>();
        RunVec4SwapRunTimeTests<f32>();
        RunVec4SwapRunTimeTests<f64>();
    }
}
