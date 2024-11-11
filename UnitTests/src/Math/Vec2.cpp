#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Vec2", "[math][vec][vec2]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec2 = TRAP::Math::tVec2<TestType>;

    SECTION("Typedefs")
    {
        STATIC_REQUIRE(std::same_as<typename Vec2::value_type, TestType>);
        STATIC_REQUIRE(std::same_as<typename Vec2::pointer, TestType*>);
        STATIC_REQUIRE(std::same_as<typename Vec2::const_pointer, const TestType*>);
        STATIC_REQUIRE(std::same_as<typename Vec2::reference, TestType&>);
        STATIC_REQUIRE(std::same_as<typename Vec2::const_reference, const TestType&>);
        STATIC_REQUIRE(std::same_as<typename Vec2::iterator, typename std::array<TestType, 2u>::iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec2::const_iterator, typename std::array<TestType, 2u>::const_iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec2::size_type, u32>);
        STATIC_REQUIRE(std::same_as<typename Vec2::difference_type, isize>);
        STATIC_REQUIRE(std::same_as<typename Vec2::reverse_iterator, typename std::array<TestType, 2u>::reverse_iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec2::const_reverse_iterator, typename std::array<TestType, 2u>::const_reverse_iterator>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr Vec2 v{};

            STATIC_REQUIRE(v.x() == TestType(0));
            STATIC_REQUIRE(v.y() == TestType(0));
        }

        //Copy constructor
        {
            static constexpr Vec2 v{TestType(2), TestType(5)};
            static constexpr Vec2 vCopy(v);

            STATIC_REQUIRE(vCopy.x() == TestType(2));
            STATIC_REQUIRE(vCopy.y() == TestType(5));
        }

        //Move constructor
        {
            static constexpr Vec2 v(TestType(5), TestType(10));
            Vec2 vMove(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
        }

        //Scalar constructor
        {
            static constexpr Vec2 v(TestType(5));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(5));
        }

        //Value constructor
        {
            static constexpr Vec2 v(TestType(5), TestType(10));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(10));
        }

        //Conversion constructor
        {
            static constexpr Vec2 v(u64(100), u8(5));

            STATIC_REQUIRE(v.x() == TestType(100));
            STATIC_REQUIRE(v.y() == TestType(5));
        }

        //Conversion Vec2 constructor
        {
            static constexpr Vec2 v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
        }

        //Conversion Vec3 constructor
        {
            static constexpr Vec2 v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
        }

        //Conversion Vec4 constructor
        {
            static constexpr Vec2 v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
        }
    }

    SECTION("Assignments")
    {
        //Move assignment operator
        {
            static constexpr Vec2 v(TestType(5), TestType(10));
            Vec2 vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
        }

        //Copy assignment operator
        {
            static constexpr Vec2 v(TestType(5), TestType(10));
            Vec2 vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == TestType(5));
            REQUIRE(vCopy.y() == TestType(10));
        }

        //Conversion copy assignment operator
        {
            static constexpr TRAP::Math::tVec2<f64> v(f64(5.0), f64(10.0));
            Vec2 vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == TestType(5));
            REQUIRE(vConversion.y() == TestType(10));
        }
    }

    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(Vec2::Length() == 2u);
        }

        //Component access
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(5));
            //Check that v.x() returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>));

            Vec2 v2(TestType(10), TestType(5));

            REQUIRE(v2.x() == TestType(10));
            REQUIRE(v2.y() == TestType(5));
            //Check that v2.x() returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>));
        }

        //operator[]
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            STATIC_REQUIRE(v[0] == TestType(10));
            STATIC_REQUIRE(v[1] == TestType(5));
            //Check that v[0] returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>));

            Vec2 v2(TestType(10), TestType(5));

            REQUIRE(v2[0] == TestType(10));
            REQUIRE(v2[1] == TestType(5));
            //Check that v2[0] returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>));
        }

        //at()
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            STATIC_REQUIRE(v.at(0) == TestType(10));
            STATIC_REQUIRE(v.at(1) == TestType(5));
            REQUIRE_THROWS_AS(v.at(10), std::out_of_range);
            //Check that v.at(0) returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>));

            Vec2 v2(TestType(10), TestType(5));

            REQUIRE(v2.at(0) == TestType(10));
            REQUIRE(v2.at(1) == TestType(5));
            REQUIRE_THROWS_AS(v2.at(10), std::out_of_range);
            //Check that v2.at(0) returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>));
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            STATIC_REQUIRE(v.begin() == &v.x());
            STATIC_REQUIRE(v.cbegin() == &v.x());
            STATIC_REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec2::const_pointer>((&v.y()) + 1));
            STATIC_REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec2::const_pointer>((&v.y()) + 1));
            STATIC_REQUIRE(v.end() == ((&v.y()) + 1));
            STATIC_REQUIRE(v.cend() == ((&v.y()) + 1));
            STATIC_REQUIRE(v.rend() == std::reverse_iterator<typename Vec2::const_pointer>(&v.x()));
            STATIC_REQUIRE(v.crend() == std::reverse_iterator<typename Vec2::const_pointer>(&v.x()));

            Vec2 v1(TestType(10), TestType(5));

            REQUIRE(v1.begin() == &v1.x());
            REQUIRE(v1.cbegin() == &v1.x());
            REQUIRE(v1.rbegin() == std::reverse_iterator<typename Vec2::const_pointer>((&v1.y()) + 1));
            REQUIRE(v1.crbegin() == std::reverse_iterator<typename Vec2::const_pointer>((&v1.y()) + 1));
            REQUIRE(v1.end() == ((&v1.y()) + 1));
            REQUIRE(v1.cend() == ((&v1.y()) + 1));
            REQUIRE(v1.rend() == std::reverse_iterator<typename Vec2::const_pointer>(&v1.x()));
            REQUIRE(v1.crend() == std::reverse_iterator<typename Vec2::const_pointer>(&v1.x()));
        }
#endif
    }

    SECTION("Operators")
    {
        //operator==
        {
            static constexpr Vec2 v(TestType(10), TestType(5));
            static constexpr Vec2 v2(TestType(5), TestType(10));
            static constexpr Vec2 v3(TestType(10), TestType(5));

            STATIC_REQUIRE(!(v == v2));
            STATIC_REQUIRE(v == v3);
        }

        //operator!=
        {
            static constexpr Vec2 v(TestType(10), TestType(5));
            static constexpr Vec2 v2(TestType(5), TestType(10));
            static constexpr Vec2 v3(TestType(10), TestType(5));

            STATIC_REQUIRE(v != v2);
            STATIC_REQUIRE(!(v != v3));
        }

        //Unary operator+
        {
            static constexpr Vec2 v(TestType(10), TestType(5));
            static constexpr Vec2 v2 = +v;

            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(5));
        }

        //Unary operator-
        if constexpr(std::signed_integral<TestType> || std::floating_point<TestType>)
        {
            static constexpr Vec2 v(TestType(10), TestType(5));
            static constexpr Vec2 v2 = -v;

            STATIC_REQUIRE(v2.x() == TestType(-10));
            STATIC_REQUIRE(v2.y() == TestType(-5));
        }

        //Unary operator~
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(8), TestType(5));

            static constexpr Vec2 v1{~v};
            STATIC_REQUIRE(v1.x() == TestType(~v.x()));
            STATIC_REQUIRE(v1.y() == TestType(~v.y()));
        }

        //Binary operator+
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v + TestType(20)};
            STATIC_REQUIRE(v1.x() == TestType(30));
            STATIC_REQUIRE(v1.y() == TestType(25));

            static constexpr Vec2 v2{TestType(20) + v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(25));

            static constexpr Vec2 v3{v + v};
            STATIC_REQUIRE(v3.x() == TestType(20));
            STATIC_REQUIRE(v3.y() == TestType(10));
        }

        //Binary operator-
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v - TestType(5)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(0));

            static constexpr Vec2 v2{TestType(20) - v};
            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(15));

            static constexpr Vec2 v3{v - v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
        }

        //Binary operator*
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v * TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(20));
            STATIC_REQUIRE(v1.y() == TestType(10));

            static constexpr Vec2 v2{TestType(2) * v};
            STATIC_REQUIRE(v2.x() == TestType(20));
            STATIC_REQUIRE(v2.y() == TestType(10));

            static constexpr Vec2 v3{v * v};
            STATIC_REQUIRE(v3.x() == TestType(100));
            STATIC_REQUIRE(v3.y() == TestType(25));
        }

        //Binary operator/
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v / TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(2.5));

            static constexpr Vec2 v2{TestType(20) / v};
            STATIC_REQUIRE(v2.x() == TestType(2));
            STATIC_REQUIRE(v2.y() == TestType(4));

            static constexpr Vec2 v3{v / v};
            STATIC_REQUIRE(v3.x() == TestType(1));
            STATIC_REQUIRE(v3.y() == TestType(1));

            const Vec2 v4(TestType(10), TestType(5));
            const Vec2 v4_1{v4 / TestType(2)};
            REQUIRE(v4_1.x() == TestType(5));
            REQUIRE(v4_1.y() == TestType(2.5));
        }

        //Binary operator%
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v % TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(0));
            STATIC_REQUIRE(v1.y() == TestType(1));

            static constexpr Vec2 v2{TestType(20) % v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));

            static constexpr Vec2 v3{v % v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
        }

        //Binary operator&
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v & TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(0));

            static constexpr Vec2 v2{TestType(20) & v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(4));

            static constexpr Vec2 v3{v & v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
        }

        //Binary operator|
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v | TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(10));
            STATIC_REQUIRE(v1.y() == TestType(7));

            static constexpr Vec2 v2{TestType(20) | v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(21));

            static constexpr Vec2 v3{v | v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
        }

        //Binary operator^
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(10), TestType(5));

            static constexpr Vec2 v1{v ^ TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(8));
            STATIC_REQUIRE(v1.y() == TestType(7));

            static constexpr Vec2 v2{TestType(20) ^ v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(17));

            static constexpr Vec2 v3{v ^ v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
        }

        //Binary operator<<
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(1), TestType(2));

            static constexpr Vec2 v1{v << TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(4));
            STATIC_REQUIRE(v1.y() == TestType(8));

            static constexpr Vec2 v2{TestType(2) << v};
            STATIC_REQUIRE(v2.x() == TestType(4));
            STATIC_REQUIRE(v2.y() == TestType(8));

            static constexpr Vec2 v3{v << v};
            STATIC_REQUIRE(v3.x() == TestType(2));
            STATIC_REQUIRE(v3.y() == TestType(8));
        }

        //Binary operator>>
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec2 v(TestType(8), TestType(5));

            static constexpr Vec2 v1{v >> TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(1));

            static constexpr Vec2 v2{TestType(2) >> v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));

            static constexpr Vec2 v3{v >> v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
        }

        //Binary operator&&
        {
            static constexpr TRAP::Math::tVec2<bool> v(true, true);
            static constexpr TRAP::Math::tVec2<bool> v1(false, false);
            static constexpr TRAP::Math::tVec2<bool> v2(true, false);
            static constexpr TRAP::Math::tVec2<bool> v3(false, true);

            STATIC_REQUIRE((v && v) == v);
            STATIC_REQUIRE((v1 && v1) == v1);
            STATIC_REQUIRE((v2 && v2) == v2);
            STATIC_REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            static constexpr TRAP::Math::tVec2<bool> v(true, true);
            static constexpr TRAP::Math::tVec2<bool> v1(false, false);
            static constexpr TRAP::Math::tVec2<bool> v2(true, false);
            static constexpr TRAP::Math::tVec2<bool> v3(false, true);

            STATIC_REQUIRE((v || v) == v);
            STATIC_REQUIRE((v1 || v1) == v1);
            STATIC_REQUIRE((v2 || v2) == v2);
            STATIC_REQUIRE((v3 || v3) == v3);
        }

        //operator+=
        {
            Vec2 v(TestType(10), TestType(5));
            v += f64(9.0);

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            static constexpr TRAP::Math::tVec2<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
        }

        //operator-=
        {
            Vec2 v(TestType(10), TestType(5));
            v -= f64(4.0);

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            static constexpr TRAP::Math::tVec2<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
        }

        //operator*=
        {
            Vec2 v(TestType(10), TestType(5));
            v *= f64(2.0);

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            static constexpr TRAP::Math::tVec2<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
        }

        //operator/=
        {
            Vec2 v(TestType(10), TestType(5));
            v /= f64(2.0);

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            static constexpr TRAP::Math::tVec2<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
        }

        //operator++
        {
            Vec2 v(TestType(10), TestType(5));
            v++;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            ++v;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
        }

        //operator--
        {
            Vec2 v(TestType(10), TestType(5));
            v--;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            --v;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
        }

        //operator%=
        {
            Vec2 v(TestType(10), TestType(5));
            v %= f64(2.0);

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
        }
        {
            Vec2 v(TestType(10), TestType(5));
            static constexpr TRAP::Math::tVec2<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
        }

        //operator&=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec2 v(TestType(10), TestType(5));
                v &= u8(2);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
            }
            {
                Vec2 v(TestType(10), TestType(5));
                static constexpr TRAP::Math::tVec2<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec2 v(TestType(10), TestType(5));
                v |= u8(2);

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
            }
            {
                Vec2 v(TestType(10), TestType(5));
                static constexpr TRAP::Math::tVec2<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec2 v(TestType(10), TestType(5));
                v ^= u8(2.0);

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
            }
            {
                Vec2 v(TestType(10), TestType(5));
                static constexpr TRAP::Math::tVec2<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec2 v(TestType(10), TestType(5));
                v <<= u8(2.0);

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
            }
            {
                Vec2 v(TestType(10), TestType(5));
                static constexpr TRAP::Math::tVec2<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec2 v(TestType(10), TestType(5));
                v >>= u8(2.0);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
            }
            {
                Vec2 v(TestType(10), TestType(5));
                static constexpr TRAP::Math::tVec2<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
            }
        }
    }

    SECTION("std::hash")
    {
        usize hash = std::hash<Vec2>()(Vec2(TestType(5), TestType(10)));
    }

    SECTION("fmt::format")
    {
        static constexpr Vec2 v(TestType(5), TestType(10));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<TestType, f32>)
            REQUIRE(str == fmt::format("Vec2f({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, f64>)
            REQUIRE(str == fmt::format("Vec2d({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, bool>)
            REQUIRE(str == fmt::format("Vec2b({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, i8>)
            REQUIRE(str == fmt::format("Vec2i8({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, i16>)
            REQUIRE(str == fmt::format("Vec2i16({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, i32>)
            REQUIRE(str == fmt::format("Vec2i32({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, i64>)
            REQUIRE(str == fmt::format("Vec2i64({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, u8>)
            REQUIRE(str == fmt::format("Vec2u8({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, u16>)
            REQUIRE(str == fmt::format("Vec2u16({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, u32>)
            REQUIRE(str == fmt::format("Vec2u32({}, {})", v.x(), v.y()));
		else if constexpr(std::same_as<TestType, u64>)
            REQUIRE(str == fmt::format("Vec2u64({}, {})", v.x(), v.y()));
		else
			REQUIRE(str == "Unknown type");
    }

    SECTION("std::get")
    {
        static constexpr Vec2 v(TestType(5), TestType(10));

        STATIC_REQUIRE(std::get<0u>(v) == TestType(5));
        STATIC_REQUIRE(std::get<1u>(std::move(v)) == TestType(10));

        Vec2 v1(TestType(5), TestType(10));
        std::get<0u>(v1) = std::get<1u>(v1);

        REQUIRE(std::get<0u>(v1) == TestType(10));
    }

    SECTION("std::swap")
    {
        {
            Vec2 v(TestType(5), TestType(10));
            Vec2 v1(TestType(10), TestType(5));

            v.Swap(v1);

            REQUIRE(v.x() == TestType(10));
            REQUIRE(v.y() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
        }
        {
            Vec2 v(TestType(5), TestType(10));
            Vec2 v1(TestType(10), TestType(5));

            std::swap(v, v1);

            REQUIRE(v.x() == TestType(10));
            REQUIRE(v.y() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
        }
    }
}
