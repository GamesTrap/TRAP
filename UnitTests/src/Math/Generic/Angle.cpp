#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Angle()", "[math][generic][angle][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        {
            static constexpr TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            static_assert(TRAP::Math::Equal(a, Scalar(90.0f), Scalar(0.0000000000001f)));
        }
        {
            static constexpr TestType q(Vec3Scalar(0.0f, 1.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            static_assert(TRAP::Math::Equal(a, Scalar(90.0f), Scalar(0.0000000000001f)));
        }
        if constexpr(std::same_as<TestType, TRAP::Math::Quatf>)
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<Scalar>() - Scalar(1.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr f32 a = TRAP::Math::Angle(q);
            static_assert(TRAP::Math::Equal(a, 5.28f, 0.01f));
        }

        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            static constexpr Scalar a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), TRAP::Math::Normalize(Vec3Scalar(0.0f, 1.0f, 1.0f)));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            static constexpr Scalar a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), TRAP::Math::Normalize(Vec3Scalar(1.0f, 2.0f, 3.0f)));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            static constexpr Scalar a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
        }
    }

    SECTION("Normal cases - std")
    {
        {
            static constexpr TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::Equal(a, Scalar(90.0f), Scalar(0.0000000000001f)));
        }
        {
            static constexpr TestType q(Vec3Scalar(0.0f, 1.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::Equal(a, Scalar(90.0f), Scalar(0.0000000000001f)));
        }
        if constexpr(std::same_as<Scalar, f32>)
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<Scalar>() - Scalar(1.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            const Scalar a = TRAP::Math::Angle(q);
            REQUIRE(TRAP::Math::Equal(a, Scalar(5.28319f), Scalar(0.00001f)));
        }

        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            const Scalar a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), TRAP::Math::Normalize(Vec3Scalar(0.0f, 1.0f, 1.0f)));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            const Scalar a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * Scalar(0.25f), TRAP::Math::Normalize(Vec3Scalar(1.0f, 2.0f, 3.0f)));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static constexpr Scalar l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, Scalar(1.0f), Scalar(0.01f)));
            const Scalar a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<Scalar>() * Scalar(0.25f), Scalar(0.01f)));
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
            const TestType q(min, min, min, min);
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            const TestType q(max, max, max, max);
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TestType q(inf, inf, inf, inf);
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TestType q(ninf, ninf, ninf, ninf);
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TestType q(nan, nan, nan, nan);
            const Scalar a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
    }
}
