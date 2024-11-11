#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Vec3", "[math][vec][vec3]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec3 = TRAP::Math::tVec3<TestType>;

    SECTION("Typedefs")
    {
        static_assert(std::same_as<typename Vec3::value_type, TestType>);
        static_assert(std::same_as<typename Vec3::pointer, TestType*>);
        static_assert(std::same_as<typename Vec3::const_pointer, const TestType*>);
        static_assert(std::same_as<typename Vec3::reference, TestType&>);
        static_assert(std::same_as<typename Vec3::const_reference, const TestType&>);
        static_assert(std::same_as<typename Vec3::iterator, typename std::array<TestType, 3u>::iterator>);
        static_assert(std::same_as<typename Vec3::const_iterator, typename std::array<TestType, 3u>::const_iterator>);
        static_assert(std::same_as<typename Vec3::size_type, u32>);
        static_assert(std::same_as<typename Vec3::difference_type, isize>);
        static_assert(std::same_as<typename Vec3::reverse_iterator, typename std::array<TestType, 3u>::reverse_iterator>);
        static_assert(std::same_as<typename Vec3::const_reverse_iterator, typename std::array<TestType, 3u>::const_reverse_iterator>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr Vec3 v{};

            STATIC_REQUIRE(v.x() == TestType(0));
            STATIC_REQUIRE(v.y() == TestType(0));
            STATIC_REQUIRE(v.z() == TestType(0));
        }

        //Copy constructor
        {
            static constexpr Vec3 v{TestType(2), TestType(5), TestType(8)};
            static constexpr Vec3 vCopy(v);

            STATIC_REQUIRE(vCopy.x() == TestType(2));
            STATIC_REQUIRE(vCopy.y() == TestType(5));
            STATIC_REQUIRE(vCopy.z() == TestType(8));
        }

        //Move constructor
        {
            Vec3 v(TestType(5), TestType(10), TestType(15));
            Vec3 vMove(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
            REQUIRE(vMove.z() == TestType(15));
        }

        //Scalar constructor
        {
            static constexpr Vec3 v(TestType(5));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(5));
        }

        //Value constructor
        {
            static constexpr Vec3 v(TestType(5), TestType(10), TestType(15));

            STATIC_REQUIRE(v.x() == TestType(5));
            STATIC_REQUIRE(v.y() == TestType(10));
            STATIC_REQUIRE(v.z() == TestType(15));
        }

        //Conversion constructor
        {
            static constexpr Vec3 v(u64(100), u8(5), u16(8));

            STATIC_REQUIRE(v.x() == TestType(100));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(8));
        }

        //Conversion Vec2 constructor
        {
            {
                static constexpr Vec3 v(TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)), f32(3.0f));

                STATIC_REQUIRE(v.x() == TestType(10));
                STATIC_REQUIRE(v.y() == TestType(127));
                STATIC_REQUIRE(v.z() == TestType(3));
            }
            {
                static constexpr Vec3 v(f32(3.0f), TRAP::Math::tVec2<f64>(f64(10.0), f64(127.0)));

                STATIC_REQUIRE(v.x() == TestType(3));
                STATIC_REQUIRE(v.y() == TestType(10));
                STATIC_REQUIRE(v.z() == TestType(127));
            }
        }

        //Conversion Vec3 constructor
        {
            static constexpr Vec3 v(TRAP::Math::tVec3<f64>(f64(10.0), f64(127.0), f64(80.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
            STATIC_REQUIRE(v.z() == TestType(80));
        }

        //Conversion Vec4 constructor
        {
            static constexpr Vec3 v(TRAP::Math::tVec4<f64>(f64(10.0), f64(127.0), f64(80.0), f64(1.0)));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(127));
            STATIC_REQUIRE(v.z() == TestType(80));
        }
    }

    SECTION("Assignments")
    {
        //Move assignment operator
        {
            Vec3 v(TestType(5), TestType(10), TestType(15));
            Vec3 vMove{};
            vMove = std::move(v);

            REQUIRE(vMove.x() == TestType(5));
            REQUIRE(vMove.y() == TestType(10));
            REQUIRE(vMove.z() == TestType(15));
        }

        //Copy assignment operator
        {
            static constexpr Vec3 v(TestType(5), TestType(10), TestType(15));
            Vec3 vCopy{};
            vCopy = v;

            REQUIRE(vCopy.x() == TestType(5));
            REQUIRE(vCopy.y() == TestType(10));
            REQUIRE(vCopy.z() == TestType(15));
        }

        //Conversion copy assignment operator
        {
            static constexpr TRAP::Math::tVec3<f64> v(f64(5.0), f64(10.0), f64(9.0));
            Vec3 vConversion{};
            vConversion = v;

            REQUIRE(vConversion.x() == TestType(5));
            REQUIRE(vConversion.y() == TestType(10));
            REQUIRE(vConversion.z() == TestType(9));
        }
    }


    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(Vec3::Length() == 3u);
        }

        //Component access
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(v.x() == TestType(10));
            STATIC_REQUIRE(v.y() == TestType(5));
            STATIC_REQUIRE(v.z() == TestType(15));
            //Check that v.x() returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>));

            Vec3 v2(TestType(10), TestType(5), TestType(15));

            REQUIRE(v2.x() == TestType(10));
            REQUIRE(v2.y() == TestType(5));
            REQUIRE(v2.z() == TestType(15));
            //Check that v2.x() returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>));
        }

        //operator[]
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(v[0] == TestType(10));
            STATIC_REQUIRE(v[1] == TestType(5));
            STATIC_REQUIRE(v[2] == TestType(15));
            //Check that v[0] returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>));

            Vec3 v2(TestType(10), TestType(5), TestType(15));

            REQUIRE(v2[0] == TestType(10));
            REQUIRE(v2[1] == TestType(5));
            REQUIRE(v2[2] == TestType(15));
            //Check that v2[0] returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>));
        }

        //at()
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(v.at(0) == TestType(10));
            STATIC_REQUIRE(v.at(1) == TestType(5));
            STATIC_REQUIRE(v.at(2) == TestType(15));
            REQUIRE_THROWS_AS(v.at(10), std::out_of_range);
            //Check that v.at(0) returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>));

            Vec3 v2(TestType(10), TestType(5), TestType(15));

            REQUIRE(v2.at(0) == TestType(10));
            REQUIRE(v2.at(1) == TestType(5));
            REQUIRE(v2.at(2) == TestType(15));
            REQUIRE_THROWS_AS(v2.at(10), std::out_of_range);
            //Check that v2.at(0) returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>));
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(v.begin() == &v.x());
            STATIC_REQUIRE(v.cbegin() == &v.x());
            STATIC_REQUIRE(v.rbegin() == std::reverse_iterator<typename Vec3::const_pointer>((&v.z()) + 1));
            STATIC_REQUIRE(v.crbegin() == std::reverse_iterator<typename Vec3::const_pointer>((&v.z()) + 1));
            STATIC_REQUIRE(v.end() == ((&v.z()) + 1));
            STATIC_REQUIRE(v.cend() == ((&v.z()) + 1));
            STATIC_REQUIRE(v.rend() == std::reverse_iterator<typename Vec3::const_pointer>(&v.x()));
            STATIC_REQUIRE(v.crend() == std::reverse_iterator<typename Vec3::const_pointer>(&v.x()));

            Vec3 v1(TestType(10), TestType(5), TestType(15));

            REQUIRE(v1.begin() == &v1.x());
            REQUIRE(v1.cbegin() == &v1.x());
            REQUIRE(v1.rbegin() == std::reverse_iterator<typename Vec3::const_pointer>((&v1.z()) + 1));
            REQUIRE(v1.crbegin() == std::reverse_iterator<typename Vec3::const_pointer>((&v1.z()) + 1));
            REQUIRE(v1.end() == ((&v1.z()) + 1));
            REQUIRE(v1.cend() == ((&v1.z()) + 1));
            REQUIRE(v1.rend() == std::reverse_iterator<typename Vec3::const_pointer>(&v1.x()));
            REQUIRE(v1.crend() == std::reverse_iterator<typename Vec3::const_pointer>(&v1.x()));
        }
#endif
    }

    SECTION("Operators")
    {
        //operator==
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr Vec3 v2(TestType(15), TestType(5), TestType(10));
            static constexpr Vec3 v3(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(!(v == v2));
            STATIC_REQUIRE(v == v3);
        }

        //operator!=
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr Vec3 v2(TestType(15), TestType(5), TestType(10));
            static constexpr Vec3 v3(TestType(10), TestType(5), TestType(15));

            STATIC_REQUIRE(v != v2);
            STATIC_REQUIRE(!(v != v3));
        }

        //Unary operator+
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr Vec3 v2 = +v;

            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(5));
            STATIC_REQUIRE(v2.z() == TestType(15));
        }

        //Unary operator-
        if constexpr(std::signed_integral<TestType> || std::floating_point<TestType>)
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr Vec3 v2 = -v;

            STATIC_REQUIRE(v2.x() == TestType(-10));
            STATIC_REQUIRE(v2.y() == TestType(-5));
            STATIC_REQUIRE(v2.z() == TestType(-15));
        }

        //Unary operator~
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(8), TestType(5), TestType(10));

            static constexpr Vec3 v1{~v};
            STATIC_REQUIRE(v1.x() == TestType(~v.x()));
            STATIC_REQUIRE(v1.y() == TestType(~v.y()));
            STATIC_REQUIRE(v1.z() == TestType(~v.z()));
        }

        //Binary operator+
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v + TestType(20)};
            STATIC_REQUIRE(v1.x() == TestType(30));
            STATIC_REQUIRE(v1.y() == TestType(25));
            STATIC_REQUIRE(v1.z() == TestType(35));

            static constexpr Vec3 v2{TestType(20) + v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(25));
            STATIC_REQUIRE(v2.z() == TestType(35));

            static constexpr Vec3 v3{v + v};
            STATIC_REQUIRE(v3.x() == TestType(20));
            STATIC_REQUIRE(v3.y() == TestType(10));
            STATIC_REQUIRE(v3.z() == TestType(30));
        }

        //Binary operator-
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v - TestType(5)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(0));
            STATIC_REQUIRE(v1.z() == TestType(10));

            static constexpr Vec3 v2{TestType(20) - v};
            STATIC_REQUIRE(v2.x() == TestType(10));
            STATIC_REQUIRE(v2.y() == TestType(15));
            STATIC_REQUIRE(v2.z() == TestType(5));

            static constexpr Vec3 v3{v - v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
        }

        //Binary operator*
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(2));

            static constexpr Vec3 v1{v * TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(20));
            STATIC_REQUIRE(v1.y() == TestType(10));
            STATIC_REQUIRE(v1.z() == TestType(4));

            static constexpr Vec3 v2{TestType(2) * v};
            STATIC_REQUIRE(v2.x() == TestType(20));
            STATIC_REQUIRE(v2.y() == TestType(10));
            STATIC_REQUIRE(v2.z() == TestType(4));

            static constexpr Vec3 v3{v * v};
            STATIC_REQUIRE(v3.x() == TestType(100));
            STATIC_REQUIRE(v3.y() == TestType(25));
            STATIC_REQUIRE(v3.z() == TestType(4));
        }

        //Binary operator/
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(20));

            static constexpr Vec3 v1{v / TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(5));
            STATIC_REQUIRE(v1.y() == TestType(2.5));
            STATIC_REQUIRE(v1.z() == TestType(10));

            static constexpr Vec3 v2{TestType(20) / v};
            STATIC_REQUIRE(v2.x() == TestType(2));
            STATIC_REQUIRE(v2.y() == TestType(4));
            STATIC_REQUIRE(v2.z() == TestType(1));

            static constexpr Vec3 v3{v / v};
            STATIC_REQUIRE(v3.x() == TestType(1));
            STATIC_REQUIRE(v3.y() == TestType(1));
            STATIC_REQUIRE(v3.z() == TestType(1));
        }

        //Binary operator%
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v % TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(0));
            STATIC_REQUIRE(v1.y() == TestType(1));
            STATIC_REQUIRE(v1.z() == TestType(1));

            static constexpr Vec3 v2{TestType(20) % v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));
            STATIC_REQUIRE(v2.z() == TestType(5));

            static constexpr Vec3 v3{v % v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
        }

        //Binary operator&
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v & TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(0));
            STATIC_REQUIRE(v1.z() == TestType(2));

            static constexpr Vec3 v2{TestType(20) & v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(4));
            STATIC_REQUIRE(v2.z() == TestType(4));

            static constexpr Vec3 v3{v & v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
            STATIC_REQUIRE(v3.z() == TestType(15));
        }

        //Binary operator|
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v | TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(10));
            STATIC_REQUIRE(v1.y() == TestType(7));
            STATIC_REQUIRE(v1.z() == TestType(15));

            static constexpr Vec3 v2{TestType(20) | v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(21));
            STATIC_REQUIRE(v2.z() == TestType(31));

            static constexpr Vec3 v3{v | v};
            STATIC_REQUIRE(v3.x() == TestType(10));
            STATIC_REQUIRE(v3.y() == TestType(5));
            STATIC_REQUIRE(v3.z() == TestType(15));
        }

        //Binary operator^
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(10), TestType(5), TestType(15));

            static constexpr Vec3 v1{v ^ TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(8));
            STATIC_REQUIRE(v1.y() == TestType(7));
            STATIC_REQUIRE(v1.z() == TestType(13));

            static constexpr Vec3 v2{TestType(20) ^ v};
            STATIC_REQUIRE(v2.x() == TestType(30));
            STATIC_REQUIRE(v2.y() == TestType(17));
            STATIC_REQUIRE(v2.z() == TestType(27));

            static constexpr Vec3 v3{v ^ v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
        }

        //Binary operator<<
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(1), TestType(2), TestType(3));

            static constexpr Vec3 v1{v << TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(4));
            STATIC_REQUIRE(v1.y() == TestType(8));
            STATIC_REQUIRE(v1.z() == TestType(12));

            static constexpr Vec3 v2{TestType(2) << v};
            STATIC_REQUIRE(v2.x() == TestType(4));
            STATIC_REQUIRE(v2.y() == TestType(8));
            STATIC_REQUIRE(v2.z() == TestType(16));

            static constexpr Vec3 v3{v << v};
            STATIC_REQUIRE(v3.x() == TestType(2));
            STATIC_REQUIRE(v3.y() == TestType(8));
            STATIC_REQUIRE(v3.z() == TestType(24));
        }

        //Binary operator>>
        if constexpr(std::integral<TestType>)
        {
            static constexpr Vec3 v(TestType(8), TestType(5), TestType(15));

            static constexpr Vec3 v1{v >> TestType(2)};
            STATIC_REQUIRE(v1.x() == TestType(2));
            STATIC_REQUIRE(v1.y() == TestType(1));
            STATIC_REQUIRE(v1.z() == TestType(3));

            static constexpr Vec3 v2{TestType(2) >> v};
            STATIC_REQUIRE(v2.x() == TestType(0));
            STATIC_REQUIRE(v2.y() == TestType(0));
            STATIC_REQUIRE(v2.z() == TestType(0));

            static constexpr Vec3 v3{v >> v};
            STATIC_REQUIRE(v3.x() == TestType(0));
            STATIC_REQUIRE(v3.y() == TestType(0));
            STATIC_REQUIRE(v3.z() == TestType(0));
        }

        //Binary operator&&
        {
            static constexpr TRAP::Math::tVec3<bool> v(true, true, true);
            static constexpr TRAP::Math::tVec3<bool> v1(false, false, false);
            static constexpr TRAP::Math::tVec3<bool> v2(true, false, true);
            static constexpr TRAP::Math::tVec3<bool> v3(false, true, false);

            STATIC_REQUIRE((v && v) == v);
            STATIC_REQUIRE((v1 && v1) == v1);
            STATIC_REQUIRE((v2 && v2) == v2);
            STATIC_REQUIRE((v3 && v3) == v3);
        }

        //Binary operator||
        {
            static constexpr TRAP::Math::tVec3<bool> v(true, true, true);
            static constexpr TRAP::Math::tVec3<bool> v1(false, false, false);
            static constexpr TRAP::Math::tVec3<bool> v2(true, false, true);
            static constexpr TRAP::Math::tVec3<bool> v3(false, true, false);

            STATIC_REQUIRE((v || v) == v);
            STATIC_REQUIRE((v1 || v1) == v1);
            STATIC_REQUIRE((v2 || v2) == v2);
            STATIC_REQUIRE((v3 || v3) == v3);
        }

        //operator+=
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v += f64(9.0);

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
            REQUIRE(v.z() == TestType(24));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr TRAP::Math::tVec3<f64> v2(f64(9.0));
            v += v2;

            REQUIRE(v.x() == TestType(19));
            REQUIRE(v.y() == TestType(14));
            REQUIRE(v.z() == TestType(24));
        }

        //operator-=
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v -= f64(4.0);

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(11));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr TRAP::Math::tVec3<f64> v2(f64(4.0));
            v -= v2;

            REQUIRE(v.x() == TestType(6));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(11));
        }

        //operator*=
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v *= f64(2.0);

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(30));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr TRAP::Math::tVec3<f64> v2(f64(2.0));
            v *= v2;

            REQUIRE(v.x() == TestType(20));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(30));
        }

        //operator/=
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v /= f64(2.0);

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
            REQUIRE(v.z() == TestType(7.5));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr TRAP::Math::tVec3<f64> v2(f64(2.0));
            v /= v2;

            REQUIRE(v.x() == TestType(5));
            REQUIRE(v.y() == TestType(2.5));
            REQUIRE(v.z() == TestType(7.5));
        }

        //operator++
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v++;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
            REQUIRE(v.z() == TestType(16));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            ++v;

            REQUIRE(v.x() == TestType(11));
            REQUIRE(v.y() == TestType(6));
            REQUIRE(v.z() == TestType(16));
        }

        //operator--
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v--;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
            REQUIRE(v.z() == TestType(14));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            --v;

            REQUIRE(v.x() == TestType(9));
            REQUIRE(v.y() == TestType(4));
            REQUIRE(v.z() == TestType(14));
        }

        //operator%=
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            v %= f64(2.0);

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(1));
        }
        {
            Vec3 v(TestType(10), TestType(5), TestType(15));
            static constexpr TRAP::Math::tVec3<f64> v2(f64(2.0));
            v %= v2;

            REQUIRE(v.x() == TestType(0));
            REQUIRE(v.y() == TestType(1));
            REQUIRE(v.z() == TestType(1));
        }

        //operator&=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                v &= u8(2);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
                REQUIRE(v.z() == TestType(2));
            }
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                static constexpr TRAP::Math::tVec3<u8> v2(u8(2));
                v &= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(0));
                REQUIRE(v.z() == TestType(2));
            }
        }

        //operator|=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                v |= u8(2);

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(15));
            }
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                static constexpr TRAP::Math::tVec3<u8> v2(u8(2));
                v |= v2;

                REQUIRE(v.x() == TestType(10));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(15));
            }
        }

        //operator^=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                v ^= u8(2.0);

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(13));
            }
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                static constexpr TRAP::Math::tVec3<u8> v2(u8(2.0));
                v ^= v2;

                REQUIRE(v.x() == TestType(8));
                REQUIRE(v.y() == TestType(7));
                REQUIRE(v.z() == TestType(13));
            }
        }

        //operator<<=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                v <<= u8(2.0);

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
                REQUIRE(v.z() == TestType(60));
            }
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                static constexpr TRAP::Math::tVec3<u8> v2(u8(2.0));
                v <<= v2;

                REQUIRE(v.x() == TestType(40));
                REQUIRE(v.y() == TestType(20));
                REQUIRE(v.z() == TestType(60));
            }
        }

        //operator>>=
        if constexpr(std::is_integral_v<TestType>)
        {
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                v >>= u8(2.0);

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
                REQUIRE(v.z() == TestType(3));
            }
            {
                Vec3 v(TestType(10), TestType(5), TestType(15));
                static constexpr TRAP::Math::tVec3<u8> v2(u8(2.0));
                v >>= v2;

                REQUIRE(v.x() == TestType(2));
                REQUIRE(v.y() == TestType(1));
                REQUIRE(v.z() == TestType(3));
            }
        }
    }

    SECTION("std::hash")
    {
        usize hash = std::hash<Vec3>()(Vec3(TestType(5), TestType(10), TestType(15)));
    }

    SECTION("fmt::format")
    {
        static constexpr Vec3 v(TestType(5), TestType(10), TestType(15));
        std::string str = fmt::format("{}", v);

		if constexpr(std::same_as<TestType, f32>)
            REQUIRE(str == fmt::format("Vec3f({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, f64>)
            REQUIRE(str == fmt::format("Vec3d({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, bool>)
            REQUIRE(str == fmt::format("Vec3b({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, i8>)
            REQUIRE(str == fmt::format("Vec3i8({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, i16>)
            REQUIRE(str == fmt::format("Vec3i16({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, i32>)
            REQUIRE(str == fmt::format("Vec3i32({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, i64>)
            REQUIRE(str == fmt::format("Vec3i64({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, u8>)
            REQUIRE(str == fmt::format("Vec3u8({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, u16>)
            REQUIRE(str == fmt::format("Vec3u16({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, u32>)
            REQUIRE(str == fmt::format("Vec3u32({}, {}, {})", v.x(), v.y(), v.z()));
		else if constexpr(std::same_as<TestType, u64>)
            REQUIRE(str == fmt::format("Vec3u64({}, {}, {})", v.x(), v.y(), v.z()));
		else
			REQUIRE(str == "Unknown type");
    }

    SECTION("std::get")
    {
        static constexpr Vec3 v(TestType(5), TestType(10), TestType(15));

        STATIC_REQUIRE(std::get<0u>(v) == TestType(5));
        STATIC_REQUIRE(std::get<1u>(std::move(v)) == TestType(10));
    }

    SECTION("std::swap")
    {
        {
            Vec3 v(TestType(5), TestType(10), TestType(15));
            Vec3 v1(TestType(15), TestType(10), TestType(5));

            v.Swap(v1);

            REQUIRE(v.x() == TestType(15));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
            REQUIRE(v1.z() == TestType(15));
        }
        {
            Vec3 v(TestType(5), TestType(10), TestType(15));
            Vec3 v1(TestType(15), TestType(10), TestType(5));

            std::swap(v, v1);

            REQUIRE(v.x() == TestType(15));
            REQUIRE(v.y() == TestType(10));
            REQUIRE(v.z() == TestType(5));
            REQUIRE(v1.x() == TestType(5));
            REQUIRE(v1.y() == TestType(10));
            REQUIRE(v1.z() == TestType(15));
        }
    }
}
