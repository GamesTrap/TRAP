#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Cos()", "[math][generic][cos][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(-1.5)), TestType(0.0707372), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(0.0)), TestType(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(0.001)), TestType(1.0), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(1.001)), TestType(0.539461), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(1.5)), TestType(0.0707372), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(11.1)), TestType(0.104236), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(50.0)), TestType(0.964966), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(TestType(150.0)), TestType(0.699251), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 8u> values
        {
            TestType(-1.5), TestType(0.0), TestType(0.001), TestType(1.001), TestType(1.5), TestType(11.1), TestType(50.0), TestType(150.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Cos(val), std::cos(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Cos()", "[math][generic][cos][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(-1.5f))), TestType(0.0707372f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(0.0f))), TestType(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(0.001f))), TestType(1.0f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(1.001f))), TestType(0.539461f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(1.5f))), TestType(0.0707372f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(11.1f))), TestType(0.104236f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(50.0f))), TestType(0.964966f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(TestType(Scalar(150.0f))), TestType(0.699251f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 8u> values
        {
            TestType(Scalar(-1.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)), TestType(Scalar(1.001)),
            TestType(Scalar(1.5)), TestType(Scalar(11.1)), TestType(Scalar(50.0)), TestType(Scalar(150.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::Cos(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::cos(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(TestType(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(TestType(ninf)))));
    }
}
