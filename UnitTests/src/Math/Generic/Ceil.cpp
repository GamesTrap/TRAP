#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Ceil()", "[math][generic][ceil][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.0f)), TestType(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.0f)), TestType(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.1f)), TestType(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.1f)), TestType(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.5f)), TestType(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.5f)), TestType(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.9f)), TestType(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.9f)), TestType(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.0f)), TestType(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.0f)), TestType(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.5f)), TestType(    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.5f)), TestType(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.9f)), TestType(    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.9f)), TestType(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.2f)), TestType(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.5f)), TestType(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.7f)), TestType(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   5.0f)), TestType(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.2f)), TestType(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.5f)), TestType(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.7f)), TestType(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  5.0f)), TestType(-   5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<TestType, 24u> values
        {
            static_cast<TestType>(0.0f), static_cast<TestType>(-0.0f), static_cast<TestType>(0.1f), static_cast<TestType>(-0.1f),
            static_cast<TestType>(0.5f), static_cast<TestType>(-0.5f), static_cast<TestType>(0.9f), static_cast<TestType>(-0.9f),
            static_cast<TestType>(1.0f), static_cast<TestType>(-1.0f), static_cast<TestType>(1.5f), static_cast<TestType>(-1.5f),
            static_cast<TestType>(1.9f), static_cast<TestType>(-1.9f), static_cast<TestType>(4.2f), static_cast<TestType>(4.5f),
            static_cast<TestType>(4.7f), static_cast<TestType>(5.0f), static_cast<TestType>(-4.2f), static_cast<TestType>(-4.5f),
            static_cast<TestType>(-4.7f), static_cast<TestType>(-5.0f), static_cast<TestType>(42e32f), static_cast<TestType>(-42e32f)
        };

        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Ceil(val), TestType(std::ceil(val)), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(max), max, Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(min), min, Epsilon));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Ceil(nan)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(inf), inf));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(ninf), ninf));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Ceil()", "[math][generic][ceil][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.0f)), TestType(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.0f)), TestType(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.1f)), TestType(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.1f)), TestType(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.5f)), TestType(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.5f)), TestType(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   0.9f)), TestType(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  0.9f)), TestType(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.0f)), TestType(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.0f)), TestType(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.5f)), TestType(    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.5f)), TestType(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   1.9f)), TestType(    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  1.9f)), TestType(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.2f)), TestType(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.5f)), TestType(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   4.7f)), TestType(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(   5.0f)), TestType(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.2f)), TestType(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.5f)), TestType(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  4.7f)), TestType(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-  5.0f)), TestType(-   5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType( 42e32f)), TestType( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(TestType(-42e32f)), TestType(-4.2e+33f), Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<TestType, 24u> values
        {
            static_cast<TestType>(0.0f), static_cast<TestType>(-0.0f), static_cast<TestType>(0.1f), static_cast<TestType>(-0.1f),
            static_cast<TestType>(0.5f), static_cast<TestType>(-0.5f), static_cast<TestType>(0.9f), static_cast<TestType>(-0.9f),
            static_cast<TestType>(1.0f), static_cast<TestType>(-1.0f), static_cast<TestType>(1.5f), static_cast<TestType>(-1.5f),
            static_cast<TestType>(1.9f), static_cast<TestType>(-1.9f), static_cast<TestType>(4.2f), static_cast<TestType>(4.5f),
            static_cast<TestType>(4.7f), static_cast<TestType>(5.0f), static_cast<TestType>(-4.2f), static_cast<TestType>(-4.5f),
            static_cast<TestType>(-4.7f), static_cast<TestType>(-5.0f), static_cast<TestType>(42e32f), static_cast<TestType>(-42e32f)
        };

        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();
        for(const TestType& val : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(val), TestType(std::ceil(std::get<0u>(val))), Epsilon)));
    }
}
