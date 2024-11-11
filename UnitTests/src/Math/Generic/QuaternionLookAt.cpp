#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::QuaternionLookAt()", "[math][generic][quaternionlookat][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Vec3Scalar eye(0.0f);
        static constexpr Vec3Scalar center(1.1f, -2.0f, 3.1416f);
        static constexpr Vec3Scalar up(-0.17f, 7.23f, -1.744f);

        static constexpr TestType q = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up);
        static constexpr TestType m = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

        static_assert(TRAP::Math::Equal(TRAP::Math::Length(q), Scalar(1.0f), Epsilon));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(-q, m, Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Vec3Scalar eye(0.0f);
        static constexpr Vec3Scalar center(1.1f, -2.0f, 3.1416f);
        static constexpr Vec3Scalar up(-0.17f, 7.23f, -1.744f);

        const TestType q = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up);
        static constexpr TestType m = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(q), Scalar(1.0f), Epsilon));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-q, m, Epsilon)));
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr Vec3Scalar eye(min);
            static constexpr Vec3Scalar target(min);
            const TestType res = TRAP::Math::QuaternionLookAt(eye, target);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr Vec3Scalar eye(max);
            static constexpr Vec3Scalar target(max);
            const TestType res = TRAP::Math::QuaternionLookAt(eye, target);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr Vec3Scalar eye(inf);
            static constexpr Vec3Scalar target(inf);
            const TestType res = TRAP::Math::QuaternionLookAt(eye, target);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr Vec3Scalar eye(ninf);
            static constexpr Vec3Scalar target(ninf);
            const TestType res = TRAP::Math::QuaternionLookAt(eye, target);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr Vec3Scalar eye(nan);
            static constexpr Vec3Scalar target(nan);
            const TestType res = TRAP::Math::QuaternionLookAt(eye, target);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
    }
}
