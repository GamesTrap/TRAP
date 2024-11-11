#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Round()", "[math][generic][round][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.5f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.5f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.9f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.9f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.0f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.0f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.9f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.9f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.2f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.5f)), TestType( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.7f)), TestType( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(   5.0f)), TestType( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.2f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.5f)), TestType(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.7f)), TestType(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  5.0f)), TestType(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 24u> values
        {
            TestType(0.0f), TestType(-0.0f), TestType(0.1f), TestType(-0.1f), TestType(0.5f), TestType(-0.5f), TestType(0.9f), TestType(-0.9f), TestType(1.0f), TestType(-1.0f),
            TestType(1.5f), TestType(-1.5f), TestType(1.9f), TestType(-1.9f), TestType(4.2f), TestType(4.5f), TestType(4.7f), TestType(5.0f), TestType(-4.2f), TestType(-4.5f),
            TestType(-4.7f), TestType(-5.0f), TestType(42e32f), TestType(-42e32f)
        };

        for(const auto val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(val), TestType(std::round(val)), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(max), max, Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(min), min, Epsilon));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Round(nan)));
        STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(inf)));
        STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Round()", "[math][generic][round][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.5f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.5f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   0.9f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  0.9f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.0f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.0f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   1.9f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  1.9f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.2f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.5f)), TestType( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   4.7f)), TestType( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(   5.0f)), TestType( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.2f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.5f)), TestType(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  4.7f)), TestType(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-  5.0f)), TestType(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 24u> values
        {
            TestType(0.0f), TestType(-0.0f), TestType(0.1f), TestType(-0.1f), TestType(0.5f), TestType(-0.5f), TestType(0.9f), TestType(-0.9f), TestType(1.0f), TestType(-1.0f),
            TestType(1.5f), TestType(-1.5f), TestType(1.9f), TestType(-1.9f), TestType(4.2f), TestType(4.5f), TestType(4.7f), TestType(5.0f), TestType(-4.2f), TestType(-4.5f),
            TestType(-4.7f), TestType(-5.0f), TestType(42e32f), TestType(-42e32f)
        };

        for(const auto& val : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(val), TestType(std::round(std::get<0>(val))), Epsilon)));
    }
}
