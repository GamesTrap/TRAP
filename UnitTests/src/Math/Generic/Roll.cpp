#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Roll()", "[math][generic][roll][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr Scalar expected = 0.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            static constexpr Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            static constexpr Scalar expected = 90.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            static constexpr Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, Scalar(0.0000000000001f)));
        }
        {
            static constexpr Scalar expected = 45.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            static constexpr Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, Scalar(0.00001f)));
        }
        {
            static constexpr Scalar expected = 180.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            static constexpr Scalar roll = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Roll(q)));
            static_assert(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            static constexpr Scalar expected = -45.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            static constexpr Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, Scalar(0.00001f)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr Scalar expected = 0.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            static constexpr Scalar expected = 90.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, Scalar(0.0000000000001f)));
        }
        {
            static const Scalar expected = 45.0f;
            static const TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, Scalar(0.00001f)));
        }
        {
            static constexpr Scalar expected = 180.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Roll(q)));
            REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            static constexpr Scalar expected = -45.0f;
            static constexpr TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, Scalar(0.00001f)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr Scalar expected = inf;
            const TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
        {
            static constexpr Scalar expected = ninf;
            const TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
        {
            static constexpr Scalar expected = nan;
            const TestType q(TRAP::Math::Radians(Vec3Scalar(0.0f, 0.0f, expected)));
            const Scalar roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
        {
            static constexpr TestType input{};
            static constexpr Scalar expected = Scalar(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Roll(input), expected, epsilon));
        }
    }
}
