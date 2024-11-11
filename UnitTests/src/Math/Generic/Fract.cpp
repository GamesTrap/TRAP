#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Fract()", "[math][generic][fract][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Fract(TestType(1.5)), TestType(0.5), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 1u> values
        {
            std::pair{TestType(1.5), TestType(0.5)}
        };

        for(const auto& [val, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(val), expected, Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(max), TestType(0.0), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(min), TestType(0.0), Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Fract()", "[math][generic][fract][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(TestType(Vec4Scalar(1.1, 1.2, 1.5, 1.7))), TestType(Vec4Scalar(0.1, 0.2, 0.5, 0.7)), Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 1u> values
        {
            std::pair<TestType, TestType>{Vec4Scalar(1.1, 1.2, 1.5, 1.7),
                                          Vec4Scalar(0.1, 0.2, 0.5, 0.7)},
        };

        for(const auto& [val, expected] : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(val), expected, Epsilon)));
    }
}
