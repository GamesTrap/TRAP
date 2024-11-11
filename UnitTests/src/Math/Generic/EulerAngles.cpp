#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::EulerAngles()", "[math][generic][eulerangles][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q(1.0f, 0.0f, 0.0f, 1.0f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            static constexpr Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TestType q(1.0f, 1.0f, 1.0f, 1.0f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            static constexpr Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TestType q(1.0f, 1.0f, 1.0f, 0.5f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            static constexpr Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q(1.0f, 0.0f, 0.0f, 1.0f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TestType q(1.0f, 1.0f, 1.0f, 1.0f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TestType q(1.0f, 1.0f, 1.0f, 0.5f);
            static constexpr Scalar roll = TRAP::Math::Roll(q);
            static constexpr Scalar pitch = TRAP::Math::Pitch(q);
            static constexpr Scalar yaw = TRAP::Math::Yaw(q);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, Vec3Scalar(pitch, yaw, roll), Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType q(min, min, 1.0f, 0.5f);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TestType q(max, max, 1.0f, 0.5f);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TestType q(inf, inf, 1.0f, 0.5f);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TestType q(ninf, ninf, 1.0f, 0.5f);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TestType q(nan, nan, 1.0f, 0.5f);
            const Vec3Scalar angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
    }
}
