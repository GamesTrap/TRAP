#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ASin()", "[math][generic][asin][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(TestType(-0.5)), TestType(-0.523599), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(TestType(0.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(TestType(0.001)), TestType(0.001), TestType(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(TestType(0.5)), TestType(0.523599), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(TestType(1.0)), TestType(1.5708), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 5u> values
        {
            TestType(-0.5), TestType(0.0), TestType(0.001), TestType(0.5), TestType(1.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ASin(val), std::asin(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ASin()", "[math][generic][asin][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(TestType(Scalar(-0.5f))), TestType(-0.523599f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(TestType(Scalar(0.0f))), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(TestType(Scalar(0.001f))), TestType(0.001f), TestType(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(TestType(Scalar(0.5f))), TestType(0.523599f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(TestType(Scalar(1.0f))), TestType(1.5708f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 5u> values
        {
            TestType(Scalar(-0.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)),
            TestType(Scalar(0.5)), TestType(Scalar(1.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::ASin(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::asin(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(TestType(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(TestType(ninf)))));
    }
}
