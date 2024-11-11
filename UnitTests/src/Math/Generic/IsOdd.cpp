#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::IsOdd()", "[math][generic][isodd][scalar]", i8, i16, i32, i64, u8, u16, u32, u64)
{
    SECTION("Normal cases")
    {
        if constexpr(std::unsigned_integral<TestType>)
        {
            STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1)));
            STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(2)));
            STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(37)));

            if constexpr(sizeof(TestType) >= 4u)
            {
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(888)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1001)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(123456789)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(987654321)));
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(1234567890)));
            }
            else if constexpr(sizeof(TestType) >= 2u)
            {
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(888)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1001)));
            }
        }
        else if constexpr(std::signed_integral<TestType>)
        {
            STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(-2)));
            STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(-1)));
            STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1)));
            STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(2)));
            STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(37)));

            if constexpr(sizeof(TestType) >= 4u)
            {
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(-123456789)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(-999)));
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(888)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1001)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(123456789)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(987654321)));
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(1234567890)));
            }
            else if constexpr(sizeof(TestType) >= 2u)
            {
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(-999)));
                STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(888)));
                STATIC_REQUIRE( TRAP::Math::IsOdd(TestType(1001)));
            }
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType min = std::numeric_limits<TestType>::min();
        static constexpr TestType max = std::numeric_limits<TestType>::max();

        STATIC_REQUIRE(!TRAP::Math::IsOdd(min));
        STATIC_REQUIRE( TRAP::Math::IsOdd(max));
        STATIC_REQUIRE(!TRAP::Math::IsOdd(TestType(0)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::IsOdd()", "[math][generic][isodd][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64)
{
    using Scalar = TestType::value_type;

    if constexpr(std::unsigned_integral<Scalar>)
    {
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1))));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(2))));
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(37))));

        if constexpr(sizeof(Scalar) >= 4u)
        {
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(888))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1001))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(123456789))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(987654321))));
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(1234567890))));
        }
        else if constexpr(sizeof(Scalar) >= 2u)
        {
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(888))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1001))));
        }
    }
    else if constexpr(std::signed_integral<Scalar>)
    {
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(-2))));
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(-1))));
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1))));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(2))));
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(37))));

        if constexpr(sizeof(Scalar) >= 4u)
        {
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(-123456789))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(-999))));
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(888))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1001))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(123456789))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(987654321))));
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(1234567890))));
        }
        else if constexpr(sizeof(Scalar) >= 2u)
        {
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(-999))));
            STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsOdd(TestType(888))));
            STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsOdd(TestType(1001))));
        }
    }
}
