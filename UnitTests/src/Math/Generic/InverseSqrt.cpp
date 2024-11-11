#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::InverseSqrt()", "[math][generic][inversesqrt][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(0.5f)), TestType(1.41421f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(1.5f)), TestType(0.816497f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(2.0f)), TestType(0.707107f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(41.5f)), TestType(0.15523f), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 4u> values
        {
            std::tuple(TestType(0.5f), TestType(1.41421f)),
            std::tuple(TestType(1.5f), TestType(0.816497f)),
            std::tuple(TestType(2.0f), TestType(0.707107f)),
            std::tuple(TestType(41.5f), TestType(0.15523f))
        };

        for(const auto& [x, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(x), expected, TestType(0.00001f)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        REQUIRE(TRAP::Math::IsInf(TRAP::Math::InverseSqrt(0.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(-1.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(min)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(inf), TestType(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::InverseSqrt()", "[math][generic][inversesqrt][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases - GCEM")
    {
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(16.0f)) * TRAP::Math::Sqrt(TestType(16.0f)), TestType(1.0f), TestType(0.01f))));
    }

    SECTION("Normal cases - std")
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::InverseSqrt(TestType(16.0f)) * TRAP::Math::Sqrt(TestType(16.0f)), TestType(1.0f), TestType(0.01f))));
    }
}
