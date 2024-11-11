#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Sign()", "[math][generic][sign][scalar]", i8, i16, i32, i64, f32, f64)
{
    SECTION("Normal cases")
    {
        if constexpr (std::floating_point<TestType>)
        {
            static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 0)), TestType( 0), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 1)), TestType( 1), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 2)), TestType( 1), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 3)), TestType( 1), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-1)), TestType(-1), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-2)), TestType(-1), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-3)), TestType(-1), Epsilon));
        }
        else
        {
            STATIC_REQUIRE(TRAP::Math::Sign(std::numeric_limits<TestType>::max()) == TestType(1));
            STATIC_REQUIRE(TRAP::Math::Sign(std::numeric_limits<TestType>::min()) == TestType(-1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType( 0)) == TestType( 0));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType( 1)) == TestType( 1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType( 2)) == TestType( 1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType( 3)) == TestType( 1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType(-1)) == TestType(-1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType(-2)) == TestType(-1));
            STATIC_REQUIRE(TRAP::Math::Sign(TestType(-3)) == TestType(-1));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Sign(max), TestType(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sign(min), TestType(-1.0), Epsilon));
        if constexpr(std::floating_point<TestType>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(nan), TestType(0.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(inf), TestType(1.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(ninf), TestType(-1.0), Epsilon));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Sign()", "[math][generic][sign][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases")
    {
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 1)), TestType( 1))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 0)), TestType( 0))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 2)), TestType( 1))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType( 3)), TestType( 1))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-1)), TestType(-1))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-2)), TestType(-1))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(TestType(-3)), TestType(-1))));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        if constexpr(std::floating_point<TestType>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(max), TestType(1.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(min), TestType(-1.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(nan), TestType(0.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(inf), TestType(1.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(ninf), TestType(-1.0), Epsilon));
        }
    }
}
