#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Floor()", "[math][generic][floor][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.0f)), TestType(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.0f)), TestType(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.1f)), TestType(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.1f)), TestType(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.5f)), TestType(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.5f)), TestType(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.9f)), TestType(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.9f)), TestType(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.0f)), TestType(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.0f)), TestType(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.1f)), TestType(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.5f)), TestType(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.5f)), TestType(-    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.9f)), TestType(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.9f)), TestType(-    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.2f)), TestType(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.5f)), TestType(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.7f)), TestType(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   5.0f)), TestType(     5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.2f)), TestType(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.5f)), TestType(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.7f)), TestType(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  5.0f)), TestType(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 25u> values
        {
            TestType(0.0f), TestType(-0.0f), TestType(0.1f), TestType(-0.1f), TestType(0.5f), TestType(-0.5f), TestType(0.9f), TestType(-0.9f), TestType(1.0f), TestType(-1.0f), TestType(1.1f),
            TestType(1.5f), TestType(-1.5f), TestType(1.9f), TestType(-1.9f), TestType(4.2f), TestType(4.5f), TestType(4.7f), TestType(5.0f), TestType(-4.2f), TestType(-4.5f),
            TestType(-4.7f), TestType(-5.0f), TestType(42e32f), TestType(-42e32f)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(val), std::floor(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(max), max, Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(min), min, Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Floor(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Floor(inf)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Floor(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Floor()", "[math][generic][floor][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.0f)), TestType(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.0f)), TestType(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.1f)), TestType(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.1f)), TestType(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.5f)), TestType(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.5f)), TestType(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   0.9f)), TestType(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  0.9f)), TestType(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.0f)), TestType(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.0f)), TestType(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.1f)), TestType(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.5f)), TestType(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.5f)), TestType(-    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   1.9f)), TestType(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  1.9f)), TestType(-    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.2f)), TestType(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.5f)), TestType(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   4.7f)), TestType(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(   5.0f)), TestType(     5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.2f)), TestType(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.5f)), TestType(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  4.7f)), TestType(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-  5.0f)), TestType(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 25u> values
        {
            TestType(0.0f), TestType(-0.0f), TestType(0.1f), TestType(-0.1f), TestType(0.5f), TestType(-0.5f), TestType(0.9f), TestType(-0.9f), TestType(1.0f), TestType(-1.0f), TestType(1.1f),
            TestType(1.5f), TestType(-1.5f), TestType(1.9f), TestType(-1.9f), TestType(4.2f), TestType(4.5f), TestType(4.7f), TestType(5.0f), TestType(-4.2f), TestType(-4.5f),
            TestType(-4.7f), TestType(-5.0f), TestType(42e32f), TestType(-42e32f)
        };

        for(const TestType& val : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(val), TestType(std::floor(std::get<0u>(val))), Epsilon)));
    }
}
