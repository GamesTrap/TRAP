#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Trunc()", "[math][generic][trunc][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.5f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.5f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.9f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.9f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.0f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.0f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.5f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.5f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.9f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.9f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.2f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.5f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.7f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   5.0f)), TestType( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.2f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.5f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.7f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  5.0f)), TestType(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon));
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

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(val), std::trunc(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(max), max, Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(min), min, Epsilon));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Trunc(nan)));
        STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(inf)));
        STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Trunc()", "[math][generic][trunc][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.5f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.5f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   0.9f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  0.9f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.0f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.0f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.5f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.5f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   1.9f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  1.9f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.2f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.5f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   4.7f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(   5.0f)), TestType( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.2f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.5f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  4.7f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-  5.0f)), TestType(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon)));
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

        for(const TestType& val : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(val), TestType(std::trunc(std::get<0u>(val))), Epsilon)));
    }
}
