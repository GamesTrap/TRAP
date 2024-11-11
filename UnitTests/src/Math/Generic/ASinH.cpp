#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ASinH()", "[math][generic][asinh][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(-1.5)), TestType(-1.19476), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(0.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(0.001)), TestType(0.001), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(1.001)), TestType(0.882081), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(1.5)), TestType(1.19476), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(11.1)), TestType(3.10212), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(50.0)), TestType(4.60527), TestType(0.000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 7u> values
        {
            TestType(-1.5), TestType(0.0), TestType(0.001), TestType(1.001), TestType(1.5), TestType(11.1), TestType(50.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ASinH(val), std::asinh(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASinH(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ASinH()", "[math][generic][asinh][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(-1.5f)), TestType(-1.19476f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(0.0f)), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(0.001f)), TestType(0.001f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(1.001f)), TestType(0.882081f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(1.5f)), TestType(1.19476f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(11.1f)), TestType(3.10212f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(TestType(50.0f)), TestType(4.60527f), TestType(0.000001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 7u> values
        {
            TestType(Scalar(-1.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)),
            TestType(Scalar(1.001)), TestType(Scalar(1.5)), TestType(Scalar(11.1)),
            TestType(Scalar(50.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::ASinH(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::asinh(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASinH(TestType(nan)))));
    }
}
