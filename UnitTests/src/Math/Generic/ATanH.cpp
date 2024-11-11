#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ATanH()", "[math][generic][atanh][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(-0.99)), TestType(-2.64665), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(0.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(0.001)), TestType(0.001), TestType(0.000000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 3u> values
        {
            TestType(-0.99), TestType(0.0), TestType(0.001)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ATanH(val), std::atanh(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(TestType(1.1))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(TestType(-1.0))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(TestType(1.0))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ATanH()", "[math][generic][atanh][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(-0.99f)), TestType(-2.64665f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(0.0f)), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(TestType(0.001f)), TestType(0.001f), TestType(0.000000001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 3u> values
        {
            TestType(Scalar(-0.99)), TestType(Scalar(0.0)), TestType(Scalar(0.001))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::ATanH(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atanh(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(TestType(Scalar(1.1))))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(TestType(Scalar(-1.0))))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(TestType(Scalar(1.0))))));
    }
}
