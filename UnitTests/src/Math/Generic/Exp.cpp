#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Exp()", "[math][generic][exp][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(-40.0f)), TestType(0.0f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(- 4.0f)), TestType(0.0183156393f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(0.0001f)), TestType(1.0001000049976403f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.0f)), TestType(2.71828175f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.75f)), TestType(5.75460291f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.9991f)), TestType(7.382409f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(2.1f)), TestType(8.166169f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(4.0f)), TestType(54.598150033144236f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(0.0f)), TestType(1.0f), TestType(0.00001f)));

        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.0f)), TestType(TRAP::Math::e<TestType>()), TestType(0.000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 9u> values
        {
            std::tuple(TestType(-40.0f), TestType(0.0f)),
            std::tuple(TestType(-4.0f), TestType(0.0183156393f)),
            std::tuple(TestType(0.0001f), TestType(1.0001000049976403f)),
            std::tuple(TestType(1.0f), TestType(2.71828175f)),
            std::tuple(TestType(1.75f), TestType(5.75460291f)),
            std::tuple(TestType(1.9991f), TestType(7.382409f)),
            std::tuple(TestType(2.1f), TestType(8.166169f)),
            std::tuple(TestType(4.0f), TestType(54.598150033144236f)),
            std::tuple(TestType(0.0f), TestType(1.0f)),
        };

        for(const auto& [num, expected] : values)
        {
            const TestType res = TRAP::Math::Exp(num);
            REQUIRE(TRAP::Math::Equal(res, expected, TestType(0.00001f)));
        }

        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.0f)), TestType(TRAP::Math::e<TestType>()), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(ninf), TestType(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Exp(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(max)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(min), TestType(0.0f), Epsilon));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Exp()", "[math][generic][exp][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.0f)), TestType(TRAP::Math::e<Scalar>()), TestType(0.000001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(TestType(1.0f)), TestType(TRAP::Math::e<Scalar>()), Epsilon)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Exp()", "[math][generic][exp][quat]",
                   TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr TestType q(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType res(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q), res, Epsilon)));

        static constexpr TestType q2(Scalar(0.0f), Scalar(1.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType res2(TRAP::Math::Cos(Scalar(1.0f)), TRAP::Math::Sin(Scalar(1.0f)), Scalar(0.0f), Scalar(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q2), res2, Epsilon)));

        static constexpr TestType q3(Scalar(0.0f), Scalar(0.0f), Scalar(1.0f), Scalar(0.0f));
        static constexpr TestType res3(TRAP::Math::Cos(Scalar(1.0f)), Scalar(0.0f), TRAP::Math::Sin(Scalar(1.0f)), Scalar(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q3), res3, Epsilon)));

        static constexpr TestType q4(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(1.0f));
        static constexpr TestType res4(TRAP::Math::Cos(Scalar(1.0f)), Scalar(0.0f), Scalar(0.0f), TRAP::Math::Sin(Scalar(1.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q4), res4, Epsilon)));

        static constexpr TestType q6(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q6), q6, Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr TestType q(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType res(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q), res, Epsilon)));

        static constexpr TestType q2(Scalar(0.0f), Scalar(1.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType res2(TRAP::Math::Cos(Scalar(1.0f)), TRAP::Math::Sin(Scalar(1.0f)), Scalar(0.0f), Scalar(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q2), res2, Epsilon)));

        static constexpr TestType q3(Scalar(0.0f), Scalar(0.0f), Scalar(1.0f), Scalar(0.0f));
        static constexpr TestType res3(TRAP::Math::Cos(Scalar(1.0f)), Scalar(0.0f), TRAP::Math::Sin(Scalar(1.0f)), Scalar(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q3), res3, Epsilon)));

        static constexpr TestType q4(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(1.0f));
        static constexpr TestType res4(TRAP::Math::Cos(Scalar(1.0f)), Scalar(0.0f), Scalar(0.0f), TRAP::Math::Sin(Scalar(1.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q4), res4, Epsilon)));

        static constexpr TestType q6(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q6), q6, Epsilon)));
    }
}
