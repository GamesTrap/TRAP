#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::AngleAxis()", "[math][generic][angleaxis][scalar]", f64, f32)
{
    using QuatScalar = TRAP::Math::tQuat<TestType>;
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr QuatScalar a = TRAP::Math::AngleAxis(TestType(0.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
        static constexpr QuatScalar b = TRAP::Math::AngleAxis(TRAP::Math::PI<TestType>() * TestType(0.5f), Vec3Scalar(0.0f, 0.0f, 1.0f));
        static constexpr QuatScalar c = TRAP::Math::Mix(a, b, TestType(0.5f));
        static constexpr QuatScalar d = TRAP::Math::AngleAxis(TRAP::Math::PI<TestType>() * TestType(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));

        static_assert(TRAP::Math::Equal(c.x(), d.x(), TestType(0.01f)));
        static_assert(TRAP::Math::Equal(c.y(), d.y(), TestType(0.01f)));
        static_assert(TRAP::Math::Equal(c.z(), d.z(), TestType(0.01f)));
        static_assert(TRAP::Math::Equal(c.w(), d.w(), TestType(0.01f)));
    }

    SECTION("Normal cases - std")
    {
        const QuatScalar a = TRAP::Math::AngleAxis(TestType(0.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
        const QuatScalar b = TRAP::Math::AngleAxis(TRAP::Math::PI<TestType>() * TestType(0.5f), Vec3Scalar(0.0f, 0.0f, 1.0f));
        const QuatScalar c = TRAP::Math::Mix(a, b, TestType(0.5f));
        const QuatScalar d = TRAP::Math::AngleAxis(TRAP::Math::PI<TestType>() * TestType(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));

        REQUIRE(TRAP::Math::Equal(c.x(), d.x(), TestType(0.01f)));
        REQUIRE(TRAP::Math::Equal(c.y(), d.y(), TestType(0.01f)));
        REQUIRE(TRAP::Math::Equal(c.z(), d.z(), TestType(0.01f)));
        REQUIRE(TRAP::Math::Equal(c.w(), d.w(), TestType(0.01f)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        {
            const QuatScalar res = TRAP::Math::AngleAxis(inf, Vec3Scalar(0.0f, 0.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            const QuatScalar res = TRAP::Math::AngleAxis(ninf, Vec3Scalar(1.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            const QuatScalar res = TRAP::Math::AngleAxis(nan, Vec3Scalar(0.0f, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
    }
}
