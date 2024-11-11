#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::CosH()", "[math][generic][cosh][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(-0.5)), TestType(1.12763), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(0.0)), TestType(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(0.001)), TestType(1.0), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(0.5)), TestType(1.12763), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(1.0)), TestType(1.54308), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::CosH(TestType(5.0)), TestType(74.2099), TestType(0.0001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 6u> values
        {
            TestType(-0.5), TestType(0.0), TestType(0.001), TestType(0.5), TestType(1.0), TestType(5.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::CosH(val), std::cosh(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::CosH(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::CosH()", "[math][generic][cosh][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(-0.5f))), TestType(1.12763f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(0.0f))), TestType(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(0.001f))), TestType(1.0f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(0.5f))), TestType(1.12763f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(1.0f))), TestType(1.54308f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(TestType(Scalar(5.0f))), TestType(74.2099f), TestType(0.0001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 6u> values
        {
            TestType(Scalar(-0.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)),
            TestType(Scalar(0.5)), TestType(Scalar(1.0)), TestType(Scalar(5.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::CosH(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::cosh(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::CosH(TestType(nan)))));
    }
}
