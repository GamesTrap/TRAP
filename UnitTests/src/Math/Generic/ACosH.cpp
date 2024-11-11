#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ACosH()", "[math][generic][acosh][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(1.001)), TestType(0.0447176), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(1.5)), TestType(0.962424), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(11.1)), TestType(3.09806), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(50.0)), TestType(4.60507), TestType(0.000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 4u> values
        {
            TestType(1.001), TestType(1.5), TestType(11.1), TestType(50.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ACosH(val), std::acosh(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(TestType(0.5))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ACosH()", "[math][generic][acosh][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(1.001f)), TestType(0.0447176f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(1.5f)), TestType(0.962424f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(11.1f)), TestType(3.09806f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(TestType(50.0f)), TestType(4.60507f), TestType(0.000001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 4u> values
        {
            TestType(Scalar(1.001)), TestType(Scalar(1.5)),
            TestType(Scalar(11.1)), TestType(Scalar(50.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::ACosH(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::acosh(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(TestType(Scalar(0.5))))));
    }
}
