#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Vec4", "[math][vec][vec4]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec4 = TRAP::Math::tVec4<TestType>;

    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TestType>);
        STATIC_REQUIRE(std::copyable<TestType>);
        STATIC_REQUIRE(std::movable<TestType>);
    }

    SECTION("Typedefs")
    {
        STATIC_REQUIRE(std::same_as<typename Vec4::value_type, TestType>);
        STATIC_REQUIRE(std::same_as<typename Vec4::pointer, TestType*>);
        STATIC_REQUIRE(std::same_as<typename Vec4::const_pointer, const TestType*>);
        STATIC_REQUIRE(std::same_as<typename Vec4::reference, TestType&>);
        STATIC_REQUIRE(std::same_as<typename Vec4::const_reference, const TestType&>);
        STATIC_REQUIRE(std::same_as<typename Vec4::iterator, typename std::array<TestType, 4u>::iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec4::const_iterator, typename std::array<TestType, 4u>::const_iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec4::size_type, u32>);
        STATIC_REQUIRE(std::same_as<typename Vec4::difference_type, isize>);
        STATIC_REQUIRE(std::same_as<typename Vec4::reverse_iterator, typename std::array<TestType, 4u>::reverse_iterator>);
        STATIC_REQUIRE(std::same_as<typename Vec4::const_reverse_iterator, typename std::array<TestType, 4u>::const_reverse_iterator>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr Vec4 v{};

            STATIC_REQUIRE(v.x() == TestType(0));
            STATIC_REQUIRE(v.y() == TestType(0));
            STATIC_REQUIRE(v.z() == TestType(0));
            STATIC_REQUIRE(v.w() == TestType(0));
        }

        //Copy constructor
        {
            static constexpr Vec4 v{TestType(2), TestType(5), TestType(8), TestType(10)};
            static constexpr Vec4 vCopy(v);

            STATIC_REQUIRE(vCopy.x() == TestType(2));
            STATIC_REQUIRE(vCopy.y() == TestType(5));
            STATIC_REQUIRE(vCopy.z() == TestType(8));
            STATIC_REQUIRE(vCopy.w() == TestType(10));
        }

        //Move constructor
        {
            Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
            Vec4 vMove(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
            REQUIRE(vMove.z() == TestType(15));
            REQUIRE(vMove.w() == TestType(20));
        }

        //Scalar constructor
        {
            static constexpr Vec4 v(TestType(5));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(5));
            STATIC_REQUIRE(v.w() == TestType(5));
        }

        //Value constructor
        {
            static constexpr Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(10));
            STATIC_REQUIRE(v.z() == TestType(15));
            STATIC_REQUIRE(v.w() == TestType(20));
        }

        //Conversion constructor
        {
            static constexpr Vec4 v(u64(100), u8(5), u16(8), f32(10.0f));

            STATIC_REQUIRE(v.x() == TestType(100));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(8));
            STATIC_REQUIRE(v.w() == TestType(10));
        }

        //Conversion Vec2 constructor
        {
            {
                static constexpr Vec4 v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), f32(3.0f), f32(6.0f));

                STATIC_REQUIRE(v.x() == TestType(10));
                STATIC_REQUIRE(v.y() == TestType(127));
                STATIC_REQUIRE(v.z() == TestType(3));
                STATIC_REQUIRE(v.w() == TestType(6.0f));
            }
            {
                static constexpr Vec4 v(f32(3.0f), f32(6.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

                STATIC_REQUIRE(v.x() == TestType(3));
                STATIC_REQUIRE(v.y() == TestType(6));
                STATIC_REQUIRE(v.z() == TestType(10));
                STATIC_REQUIRE(v.w() == TestType(127));
            }
            {
                static constexpr Vec4 v(f32(3.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), u32(20));

                STATIC_REQUIRE(v.x() == TestType(3));
                STATIC_REQUIRE(v.y() == TestType(10));
                STATIC_REQUIRE(v.z() == TestType(127));
                STATIC_REQUIRE(v.w() == TestType(20));
            }
            {
                static constexpr Vec4 v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), TRAP::Math::tVec2<f32>(f32(10.0), f32(127.0)));

                STATIC_REQUIRE(v.x() == TestType(10));
                STATIC_REQUIRE(v.y() == TestType(127));
                STATIC_REQUIRE(v.z() == TestType(10));
                STATIC_REQUIRE(v.w() == TestType(127));
            }
        }

        //Conversion Vec3 constructor
        {
            {
                static constexpr Vec4 v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)), f32(5.0f));

                STATIC_REQUIRE(v.x() == TestType(10));
                STATIC_REQUIRE(v.y() == TestType(127));
                STATIC_REQUIRE(v.z() == TestType(80));
                STATIC_REQUIRE(v.w() == TestType(5.0f));
            }
            {
                static constexpr Vec4 v(f32(5.0f), TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

                STATIC_REQUIRE(v.x() == TestType(5.0f));
                STATIC_REQUIRE(v.y() == TestType(10));
                STATIC_REQUIRE(v.z() == TestType(127));
                STATIC_REQUIRE(v.w() == TestType(80));
            }
        }

        //Conversion Vec4 constructor
        {
            static constexpr Vec4 v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
            STATIC_REQUIRE(v.z() == TestType(80));
            STATIC_REQUIRE(v.w() == TestType(1));
        }
    }

    SECTION("Assignments")
    {
        //Move assignment operator
        {
            Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
            Vec4 vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
            REQUIRE(vMove.z() == TestType(15));
            REQUIRE(vMove.w() == TestType(20));
        }

        //Copy assignment operator
        {
            static constexpr Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
            Vec4 vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == TestType(5));
            REQUIRE(vCopy.y() == TestType(10));
            REQUIRE(vCopy.z() == TestType(15));
            REQUIRE(vCopy.w() == TestType(20));
        }

        //Conversion copy assignment operator
        {
            static constexpr TRAP::Math::tVec4<f64> v(f64(5.0), f64(10.0), f64(9.0), f64(20.0));
            Vec4 vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == TestType(5));
            REQUIRE(vConversion.y() == TestType(10));
            REQUIRE(vConversion.z() == TestType(9));
            REQUIRE(vConversion.w() == TestType(20));
        }
    }

    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(Vec4::Length() == 4u);
        }

        //Component access
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(15));
            STATIC_REQUIRE(v.w() == TestType(20));
            //Check that v.x() returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>));

            Vec4 v2(TestType(10), TestType(5), TestType(15), TestType(20));

            REQUIRE(v2.x() == TestType(10));
            REQUIRE(v2.y() == TestType(5));
            REQUIRE(v2.z() == TestType(15));
            REQUIRE(v2.w() == TestType(20));
            //Check that v2.x() returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>));
        }

        //operator[]
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(v[0] == TestType(10));
            STATIC_REQUIRE(v[1] == TestType(5));
            STATIC_REQUIRE(v[2] == TestType(15));
            STATIC_REQUIRE(v[3] == TestType(20));
            //Check that v[0] returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>));

            Vec4 v2(TestType(10), TestType(5), TestType(15), TestType(20));

            REQUIRE(v2[0] == TestType(10));
            REQUIRE(v2[1] == TestType(5));
            REQUIRE(v2[2] == TestType(15));
            REQUIRE(v2[3] == TestType(20));
            //Check that v2[0] returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>));
        }

        //at()
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(v.at(0) == TestType(10));
            STATIC_REQUIRE(v.at(1) == TestType(5));
            STATIC_REQUIRE(v.at(2) == TestType(15));
            STATIC_REQUIRE(v.at(3) == TestType(20));
            REQUIRE_THROWS_AS(v.at(10), std::out_of_range);
            //Check that v.at(0) returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>));

            Vec4 v2(TestType(10), TestType(5), TestType(15), TestType(20));

            REQUIRE(v2.at(0) == TestType(10));
            REQUIRE(v2.at(1) == TestType(5));
            REQUIRE(v2.at(2) == TestType(15));
            REQUIRE(v2.at(3) == TestType(20));
            REQUIRE_THROWS_AS(v2.at(10), std::out_of_range);
            //Check that v2.at(0) returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>));
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(v.begin() == &v.x());
            STATIC_REQUIRE(v.cbegin() == &v.x());
            STATIC_REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec4::const_pointer>((&v.w()) + 1));
            STATIC_REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec4::const_pointer>((&v.w()) + 1));
            STATIC_REQUIRE(v.end() == ((&v.w()) + 1));
            STATIC_REQUIRE(v.cend() == ((&v.w()) + 1));
            STATIC_REQUIRE(v.rend() == std::reverse_iterator<typename Vec4::const_pointer>(&v.x()));
            STATIC_REQUIRE(v.crend() == std::reverse_iterator<typename Vec4::const_pointer>(&v.x()));

            Vec4 v1(TestType(10), TestType(5), TestType(15), TestType(20));

            REQUIRE(v1.begin() == &v1.x());
            REQUIRE(v1.cbegin() == &v1.x());
            REQUIRE(v1.rbegin() == std::reverse_iterator<typename Vec4::const_pointer>((&v1.w()) + 1));
            REQUIRE(v1.crbegin() == std::reverse_iterator<typename Vec4::const_pointer>((&v1.w()) + 1));
            REQUIRE(v1.end() == ((&v1.w()) + 1));
            REQUIRE(v1.cend() == ((&v1.w()) + 1));
            REQUIRE(v1.rend() == std::reverse_iterator<typename Vec4::const_pointer>(&v1.x()));
            REQUIRE(v1.crend() == std::reverse_iterator<typename Vec4::const_pointer>(&v1.x()));
        }
#endif
    }

    SECTION("Operators")
    {
        //operator==
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr Vec4 v2(TestType(20), TestType(15), TestType(5), TestType(10));
            static constexpr Vec4 v3(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(!(v == v2));
            STATIC_REQUIRE(v == v3);
        }

        //operator!=
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr Vec4 v2(TestType(20), TestType(15), TestType(5), TestType(10));
            static constexpr Vec4 v3(TestType(10), TestType(5), TestType(15), TestType(20));

            STATIC_REQUIRE(v != v2);
            STATIC_REQUIRE(!(v != v3));
        }

        //Unary operator+
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr Vec4 v2 = +v;

            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(5));
            STATIC_REQUIRE(v2.z() == TestType(15));
            STATIC_REQUIRE(v2.w() == TestType(20));
        }

        //Unary operator-
        if constexpr(std::signed_integral<TestType> || std::floating_point<TestType>)
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr Vec4 v2 = -v;

            STATIC_REQUIRE(v2.x() == TestType(-10));
            STATIC_REQUIRE(v2.y() == TestType(-5));
            STATIC_REQUIRE(v2.z() == TestType(-15));
            STATIC_REQUIRE(v2.w() == TestType(-20));
        }

        //Unary operator~
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(8), TestType(5), TestType(10), TestType(20));

            static constexpr Vec4 v1{~v};
            STATIC_REQUIRE(v1.x() == TestType(~v.x()));
            STATIC_REQUIRE(v1.y() == TestType(~v.y()));
            STATIC_REQUIRE(v1.z() == TestType(~v.z()));
            STATIC_REQUIRE(v1.w() == TestType(~v.w()));
        }

        //Binary operator+
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v + TestType(20)};
            STATIC_REQUIRE(v1.x() == TestType(30));
            STATIC_REQUIRE(v1.y() == TestType(25));
            STATIC_REQUIRE(v1.z() == TestType(35));
            STATIC_REQUIRE(v1.w() == TestType(40));

            static constexpr Vec4 v2{TestType(20) + v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(25));
            STATIC_REQUIRE(v2.z() == TestType(35));
            STATIC_REQUIRE(v2.w() == TestType(40));

            static constexpr Vec4 v3{v + v};
            STATIC_REQUIRE(v3.x() == TestType(20));
            STATIC_REQUIRE(v3.y() == TestType(10));
            STATIC_REQUIRE(v3.z() == TestType(30));
            STATIC_REQUIRE(v3.w() == TestType(40));
        }

        //Binary operator-
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v - TestType(5)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(0));
            STATIC_REQUIRE(v1.z() == TestType(10));
            STATIC_REQUIRE(v1.w() == TestType(15));

            static constexpr Vec4 v2{TestType(20) - v};
            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(15));
            STATIC_REQUIRE(v2.z() == TestType(5));
            STATIC_REQUIRE(v2.w() == TestType(0));

            static constexpr Vec4 v3{v - v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
            STATIC_REQUIRE(v3.w() == TestType(0));
        }

        //Binary operator*
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(2), TestType(1));

            static constexpr Vec4 v1{v * TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(20));
            STATIC_REQUIRE(v1.y() == TestType(10));
            STATIC_REQUIRE(v1.z() == TestType(4));
            STATIC_REQUIRE(v1.w() == TestType(2));

            static constexpr Vec4 v2{TestType(2) * v};
            STATIC_REQUIRE(v2.x() == TestType(20));
            STATIC_REQUIRE(v2.y() == TestType(10));
            STATIC_REQUIRE(v2.z() == TestType(4));
            STATIC_REQUIRE(v2.w() == TestType(2));

            static constexpr Vec4 v3{v * v};
            STATIC_REQUIRE(v3.x() == TestType(100));
            STATIC_REQUIRE(v3.y() == TestType(25));
            STATIC_REQUIRE(v3.z() == TestType(4));
            STATIC_REQUIRE(v3.w() == TestType(1));
        }

        //Binary operator/
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(20), TestType(40));

            static constexpr Vec4 v1{v / TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(2.5));
            STATIC_REQUIRE(v1.z() == TestType(10));
            STATIC_REQUIRE(v1.w() == TestType(20));

            static constexpr Vec4 v2{TestType(20) / v};
            STATIC_REQUIRE(v2.x() == TestType(2));
            STATIC_REQUIRE(v2.y() == TestType(4));
            STATIC_REQUIRE(v2.z() == TestType(1));
            STATIC_REQUIRE(v2.w() == TestType(0.5));

            static constexpr Vec4 v3{v / v};
            STATIC_REQUIRE(v3.x() == TestType(1));
            STATIC_REQUIRE(v3.y() == TestType(1));
            STATIC_REQUIRE(v3.z() == TestType(1));
            STATIC_REQUIRE(v3.w() == TestType(1));
        }

        //Binary operator%
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v % TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(0));
            STATIC_REQUIRE(v1.y() == TestType(1));
            STATIC_REQUIRE(v1.z() == TestType(1));
            STATIC_REQUIRE(v1.w() == TestType(0));

            static constexpr Vec4 v2{TestType(20) % v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));
            STATIC_REQUIRE(v2.z() == TestType(5));
            STATIC_REQUIRE(v2.w() == TestType(0));

            static constexpr Vec4 v3{v % v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
            STATIC_REQUIRE(v3.w() == TestType(0));
        }

        //Binary operator&
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v & TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(0));
            STATIC_REQUIRE(v1.z() == TestType(2));
            STATIC_REQUIRE(v1.w() == TestType(0));

            static constexpr Vec4 v2{TestType(20) & v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(4));
            STATIC_REQUIRE(v2.z() == TestType(4));
            STATIC_REQUIRE(v2.w() == TestType(20));

            static constexpr Vec4 v3{v & v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
            STATIC_REQUIRE(v3.z() == TestType(15));
            STATIC_REQUIRE(v3.w() == TestType(20));
        }

        //Binary operator|
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v | TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(10));
            STATIC_REQUIRE(v1.y() == TestType(7));
            STATIC_REQUIRE(v1.z() == TestType(15));
            STATIC_REQUIRE(v1.w() == TestType(22));

            static constexpr Vec4 v2{TestType(20) | v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(21));
            STATIC_REQUIRE(v2.z() == TestType(31));
            STATIC_REQUIRE(v2.w() == TestType(20));

            static constexpr Vec4 v3{v | v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
            STATIC_REQUIRE(v3.z() == TestType(15));
            STATIC_REQUIRE(v3.w() == TestType(20));
        }

        //Binary operator^
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v ^ TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(8));
            STATIC_REQUIRE(v1.y() == TestType(7));
            STATIC_REQUIRE(v1.z() == TestType(13));
            STATIC_REQUIRE(v1.w() == TestType(22));

            static constexpr Vec4 v2{TestType(20) ^ v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(17));
            STATIC_REQUIRE(v2.z() == TestType(27));
            STATIC_REQUIRE(v2.w() == TestType(0));

            static constexpr Vec4 v3{v ^ v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
            STATIC_REQUIRE(v3.w() == TestType(0));
        }

        //Binary operator<<
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(1), TestType(2), TestType(3), TestType(4));

            static constexpr Vec4 v1{v << TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(4));
            STATIC_REQUIRE(v1.y() == TestType(8));
            STATIC_REQUIRE(v1.z() == TestType(12));
            STATIC_REQUIRE(v1.w() == TestType(16));

            static constexpr Vec4 v2{TestType(2) << v};
            STATIC_REQUIRE(v2.x() == TestType(4));
            STATIC_REQUIRE(v2.y() == TestType(8));
            STATIC_REQUIRE(v2.z() == TestType(16));
            STATIC_REQUIRE(v2.w() == TestType(32));

            static constexpr Vec4 v3{v << v};
            STATIC_REQUIRE(v3.x() == TestType(2));
            STATIC_REQUIRE(v3.y() == TestType(8));
            STATIC_REQUIRE(v3.z() == TestType(24));
            STATIC_REQUIRE(v3.w() == TestType(64));
        }

        //Binary operator>>
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec4 v(TestType(8), TestType(5), TestType(15), TestType(20));

            static constexpr Vec4 v1{v >> TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(1));
            STATIC_REQUIRE(v1.z() == TestType(3));
            STATIC_REQUIRE(v1.w() == TestType(5));

            static constexpr Vec4 v2{TestType(2) >> v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));
            STATIC_REQUIRE(v2.z() == TestType(0));
            STATIC_REQUIRE(v2.w() == TestType(0));

            static constexpr Vec4 v3{v >> v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
            STATIC_REQUIRE(v3.w() == TestType(0));
        }

        //Binary operator&&
        {
            static constexpr TRAP::Math::tVec4<bool> v(true, true, true, true);
            static constexpr TRAP::Math::tVec4<bool> v1(false, false, false, false);
            static constexpr TRAP::Math::tVec4<bool> v2(true, false, true, false);
            static constexpr TRAP::Math::tVec4<bool> v3(false, true, false, true);

            STATIC_REQUIRE((v && v) == v);
            STATIC_REQUIRE((v1 && v1) == v1);
            STATIC_REQUIRE((v2 && v2) == v2);
            STATIC_REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            static constexpr TRAP::Math::tVec4<bool> v(true, true, true, true);
            static constexpr TRAP::Math::tVec4<bool> v1(false, false, false, false);
            static constexpr TRAP::Math::tVec4<bool> v2(true, false, true, false);
            static constexpr TRAP::Math::tVec4<bool> v3(false, true, false, true);

            STATIC_REQUIRE((v || v) == v);
            STATIC_REQUIRE((v1 || v1) == v1);
            STATIC_REQUIRE((v2 || v2) == v2);
            STATIC_REQUIRE((v3 || v3) == v3);
        }

        //operator+=
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v += f64(9.0);

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
            REQUIRE(v.z() == TestType(24));
            REQUIRE(v.w() == TestType(29));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr TRAP::Math::tVec4<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
            REQUIRE(v.z() == TestType(24));
            REQUIRE(v.w() == TestType(29));
        }

        //operator-=
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v -= f64(4.0);

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(11));
            REQUIRE(v.w() == TestType(16));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr TRAP::Math::tVec4<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(11));
            REQUIRE(v.w() == TestType(16));
        }

        //operator*=
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v *= f64(2.0);

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(30));
            REQUIRE(v.w() == TestType(40));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr TRAP::Math::tVec4<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(30));
            REQUIRE(v.w() == TestType(40));
        }

        //operator/=
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v /= f64(2.0);

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
            REQUIRE(v.z() == TestType(7.5));
            REQUIRE(v.w() == TestType(10));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr TRAP::Math::tVec4<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
            REQUIRE(v.z() == TestType(7.5));
            REQUIRE(v.w() == TestType(10));
        }

        //operator++
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v++;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
            REQUIRE(v.z() == TestType(16));
            REQUIRE(v.w() == TestType(21));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            ++v;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
            REQUIRE(v.z() == TestType(16));
            REQUIRE(v.w() == TestType(21));
        }

        //operator--
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v--;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
            REQUIRE(v.z() == TestType(14));
            REQUIRE(v.w() == TestType(19));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            --v;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
            REQUIRE(v.z() == TestType(14));
            REQUIRE(v.w() == TestType(19));
        }

        //operator%=
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            v %= f64(2.0);

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(1));
            REQUIRE(v.w() == TestType(0));
        }
        {
            Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
            static constexpr TRAP::Math::tVec4<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(1));
            REQUIRE(v.w() == TestType(0));
        }

        //operator&=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                v &= u8(2);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
                REQUIRE(v.z() == TestType(2));
                REQUIRE(v.w() == TestType(0));
            }
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                static constexpr TRAP::Math::tVec4<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
                REQUIRE(v.z() == TestType(2));
                REQUIRE(v.w() == TestType(0));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                v |= u8(2);

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(15));
                REQUIRE(v.w() == TestType(22));
            }
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                static constexpr TRAP::Math::tVec4<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(15));
                REQUIRE(v.w() == TestType(22));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                v ^= u8(2.0);

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(13));
                REQUIRE(v.w() == TestType(22));
            }
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                static constexpr TRAP::Math::tVec4<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(13));
                REQUIRE(v.w() == TestType(22));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                v <<= u8(2.0);

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
                REQUIRE(v.z() == TestType(60));
                REQUIRE(v.w() == TestType(80));
            }
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                static constexpr TRAP::Math::tVec4<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
                REQUIRE(v.z() == TestType(60));
                REQUIRE(v.w() == TestType(80));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                v >>= u8(2.0);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
                REQUIRE(v.z() == TestType(3));
                REQUIRE(v.w() == TestType(5));
            }
            {
                Vec4 v(TestType(10), TestType(5), TestType(15), TestType(20));
                static constexpr TRAP::Math::tVec4<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
                REQUIRE(v.z() == TestType(3));
                REQUIRE(v.w() == TestType(5));
            }
        }
    }

    SECTION("std::hash")
    {
        usize hash = std::hash<Vec4>()(Vec4(TestType(5), TestType(10), TestType(15), TestType(20)));
    }

    SECTION("fmt::format")
    {
        static constexpr Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<TestType, f32>)
            REQUIRE(str == fmt::format("Vec4f({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, f64>)
            REQUIRE(str == fmt::format("Vec4d({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, bool>)
            REQUIRE(str == fmt::format("Vec4b({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, i8>)
            REQUIRE(str == fmt::format("Vec4i8({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, i16>)
            REQUIRE(str == fmt::format("Vec4i16({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, i32>)
            REQUIRE(str == fmt::format("Vec4i32({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, i64>)
            REQUIRE(str == fmt::format("Vec4i64({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, u8>)
            REQUIRE(str == fmt::format("Vec4u8({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, u16>)
            REQUIRE(str == fmt::format("Vec4u16({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, u32>)
            REQUIRE(str == fmt::format("Vec4u32({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else if constexpr(std::same_as<TestType, u64>)
            REQUIRE(str == fmt::format("Vec4u64({}, {}, {}, {})", v.x(), v.y(), v.z(), v.w()));
		else
			REQUIRE(str == "Unknown type");
    }

    SECTION("std::get")
    {
        static constexpr Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));

        STATIC_REQUIRE(std::get<0u>(v) == TestType(5));
        STATIC_REQUIRE(std::get<1u>(std::move(v)) == TestType(10));
    }

    SECTION("std::swap")
    {
        {
            Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
            Vec4 v1(TestType(20), TestType(15), TestType(10), TestType(5));

            v.Swap(v1);

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(15));
            REQUIRE(v.z() == TestType(10));
            REQUIRE(v.w() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
            REQUIRE(v1.z() == TestType(15));
            REQUIRE(v1.w() == TestType(20));
        }
        {
            Vec4 v(TestType(5), TestType(10), TestType(15), TestType(20));
            Vec4 v1(TestType(20), TestType(15), TestType(10), TestType(5));

            std::swap(v, v1);

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(15));
            REQUIRE(v.z() == TestType(10));
            REQUIRE(v.w() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
            REQUIRE(v1.z() == TestType(15));
            REQUIRE(v1.w() == TestType(20));
        }
    }
}
