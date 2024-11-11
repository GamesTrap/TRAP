#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ATan(y, x)", "[math][generic][atan2][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.2), TestType(0.0)), TestType(1.5708), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-0.2), TestType(0.0)), TestType(-1.5708), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.001), TestType(0.001)), TestType(0.785398), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.49), TestType(0.49)), TestType(0.785398), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-0.5), TestType(-0.5)), TestType(-2.35619), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.5), TestType(-0.5)), TestType(2.35619), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-0.5), TestType(0.5)), TestType(-0.785398), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(9.6), TestType(8.4)), TestType(0.851966), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.0), TestType(0.0)), TestType(1.5708), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.0), TestType(1.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-1.0), TestType(0.0)), TestType(-1.5708), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.0), TestType(-1.0)), TestType(3.14159), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.0), TestType(3.0)), TestType(0.321751), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-5.0), TestType(2.5)), TestType(-1.10715), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-1000.0), TestType(-0.001)), TestType(-1.5708), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 16u> values
        {
            std::pair(TestType(0.0), TestType(0.0)), std::pair(TestType(0.2), TestType(0.0)), std::pair(TestType(-0.2), TestType(0.0)),
            std::pair(TestType(0.001), TestType(0.001)), std::pair(TestType(0.49), TestType(0.49)), std::pair(TestType(-0.5), TestType(-0.5)),
            std::pair(TestType(0.5), TestType(-0.5)), std::pair(TestType(-0.5), TestType(0.5)), std::pair(TestType(9.6), TestType(8.4)),
            std::pair(TestType(1.0), TestType(0.0)), std::pair(TestType(0.0), TestType(1.0)), std::pair(TestType(-1.0), TestType(0.0)),
            std::pair(TestType(0.0), TestType(-1.0)), std::pair(TestType(1.0), TestType(3.0)), std::pair(TestType(-5.0), TestType(2.5)),
            std::pair(TestType(-1000.0), TestType(-0.001)),
        };

        for(const auto& [y, x] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ATan(y, x), std::atan2(y, x), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, TestType(1.0))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(TestType(1.0), nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ATan(y, x)", "[math][generic][atan2][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.2f)), TestType(Scalar(0.0f))), TestType(1.5708f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-0.2f)), TestType(Scalar(0.0f))), TestType(-1.5708f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.001f)), TestType(Scalar(0.001f))), TestType(0.785398f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.49f)), TestType(Scalar(0.49f))), TestType(0.785398f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-0.5f)), TestType(Scalar(-0.5f))), TestType(-2.35619f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.5f)), TestType(Scalar(-0.5f))), TestType(2.35619f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-0.5f)), TestType(Scalar(0.5f))), TestType(-0.785398f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(9.6f)), TestType(Scalar(8.4f))), TestType(0.851966f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(1.0f)), TestType(Scalar(0.0f))), TestType(1.5708f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.0f)), TestType(Scalar(1.0f))), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-1.0f)), TestType(Scalar(0.0f))), TestType(-1.5708f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(0.0f)), TestType(Scalar(-1.0f))), TestType(3.14159f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(1.0f)), TestType(Scalar(3.0f))), TestType(0.321751f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-5.0f)), TestType(Scalar(2.5f))), TestType(-1.10715f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(Scalar(-1000.0f)), TestType(Scalar(-0.001f))), TestType(-1.5708f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 16u> values
        {
            std::pair(TestType(Scalar(0.0)), TestType(Scalar(0.0))),
            std::pair(TestType(Scalar(0.2)), TestType(Scalar(0.0))),
            std::pair(TestType(Scalar(-0.2)), TestType(Scalar(0.0))),
            std::pair(TestType(Scalar(0.001)), TestType(Scalar(0.001))),
            std::pair(TestType(Scalar(0.49)), TestType(Scalar(0.49))),
            std::pair(TestType(Scalar(-0.5)), TestType(Scalar(-0.5))),
            std::pair(TestType(Scalar(0.5)), TestType(Scalar(-0.5))),
            std::pair(TestType(Scalar(-0.5)), TestType(Scalar(0.5))),
            std::pair(TestType(Scalar(9.6)), TestType(Scalar(8.4))),
            std::pair(TestType(Scalar(1.0)), TestType(Scalar(0.0))),
            std::pair(TestType(Scalar(0.0)), TestType(Scalar(1.0))),
            std::pair(TestType(Scalar(-1.0)), TestType(Scalar(0.0))),
            std::pair(TestType(Scalar(0.0)), TestType(Scalar(-1.0))),
            std::pair(TestType(Scalar(1.0)), TestType(Scalar(3.0))),
            std::pair(TestType(Scalar(-5.0)), TestType(Scalar(2.5))),
            std::pair(TestType(Scalar(-1000.0)), TestType(Scalar(-0.001))),
        };

        for(const auto& [y, x] : values)
        {
            const TestType s = TRAP::Math::ATan(y, x);
            for(u32 l = 0u; l < TestType::Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atan2(y[l], x[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(TestType(nan), TestType(1.0)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(TestType(1.0), TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(TestType(nan), TestType(nan)))));
    }
}
