#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::IsPowerOfTwo()", "[math][generic][ispoweroftwo][scalar]", u8, u16, u32, u64)
{
    SECTION("Normal cases")
    {
        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(TestType(0x00)));
        STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x01)));
        STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x02)));
        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(TestType(0x03)));
        STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x04)));
        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(TestType(0x0F)));

        if constexpr(sizeof(TestType) > 32u || std::same_as<TestType, u32>)
        {
            STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x80)));
            STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x80000000)));
        }
        else if constexpr(sizeof(TestType) > 8u || std::same_as<TestType, u8>)
        {
            STATIC_REQUIRE( TRAP::Math::IsPowerOfTwo(TestType(0x80)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::min();

        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(0u));
        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(max));
        STATIC_REQUIRE(!TRAP::Math::IsPowerOfTwo(min));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::IsPowerOfTwo()", "[math][generic][ispoweroftwo][vec]",
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64)
{
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x00))));
    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x01))));
    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x02))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x03))));
    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x04))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x0F))));

    if constexpr(sizeof(TestType) > 32u || std::same_as<TestType, u32>)
    {
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x80))));
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x80000000))));
    }
    else if constexpr(sizeof(TestType) > 8u || std::same_as<TestType, u8>)
    {
        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(TestType(0x80))));
    }
}
