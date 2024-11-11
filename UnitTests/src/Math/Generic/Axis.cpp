#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Axis()", "[math][generic][axis][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<typename TestType::value_type>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Epsilon)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(0.0f, 1.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(-30.0f)), Vec3Scalar(1.0f, 1.0f, 1.0f));
            static constexpr Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(-1.0f, -1.0f, -1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Scalar(0.00001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q(1.0f, 0.0f, 0.0f, 0.0f);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Epsilon)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(0.0f, 1.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(-30.0f)), Vec3Scalar(1.0f, 1.0f, 1.0f));
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            static constexpr Vec3Scalar expected(-1.0f, -1.0f, -1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Scalar(0.00001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType q(min, min, min, min);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, Vec3Scalar(0.0f, 0.0f, 1.0f), Epsilon)));
        }
        {
            static constexpr TestType q(max, max, max, max);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, Vec3Scalar(0.0f, 0.0f, 1.0f), Epsilon)));
        }
        {
            static constexpr TestType q(inf, inf, inf, inf);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, Vec3Scalar(0.0f, 0.0f, 1.0f), Epsilon)));
        }
        {
            static constexpr TestType q(ninf, ninf, ninf, ninf);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, Vec3Scalar(0.0f, 0.0f, 1.0f), Epsilon)));
        }
        {
            static constexpr TestType q(nan, nan, nan, nan);
            const Vec3Scalar axis = TRAP::Math::Axis(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(axis)));
        }
    }
}
