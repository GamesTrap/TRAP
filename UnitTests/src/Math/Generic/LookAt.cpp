#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::LookAt()", "[math][generic][lookat][scalar]", f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;
    using Mat4Scalar = TRAP::Math::tMat4<TestType>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr Vec3Scalar eye(0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            static constexpr Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr Vec3Scalar eye(2.0f);
            static constexpr Vec3Scalar center(0.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            static constexpr Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, -0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.0f, -3.464102f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, TestType(0.000001f))));
        }
        {
            static constexpr Vec3Scalar eye(-1.0f, 1.0f, 1.0f);
            static constexpr Vec3Scalar center(1.0f, -1.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            static constexpr Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(0.707107f, 0.408248f, -0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, 0.707107f, -0.408248f, 0.577350, 0.0f, 0.0f, 0.0f, -1.732051f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, TestType(0.000001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr Vec3Scalar eye(0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr Vec3Scalar eye(2.0f);
            static constexpr Vec3Scalar center(0.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, -0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.0f, -3.464102f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, TestType(0.000001f))));
        }
        {
            static constexpr Vec3Scalar eye(-1.0f, 1.0f, 1.0f);
            static constexpr Vec3Scalar center(1.0f, -1.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            static constexpr Mat4Scalar expected(0.707107f, 0.408248f, -0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, 0.707107f, -0.408248f, 0.577350, 0.0f, 0.0f, 0.0f, -1.732051f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, TestType(0.000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        {
            static constexpr Vec3Scalar eye(min, 0.0f, 0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar eye(max, 0.0f, 0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar eye(inf, 0.0f, 0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar eye(ninf, 0.0f, 0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar eye(nan, 0.0f, 0.0f);
            static constexpr Vec3Scalar center(0.0f, 0.0f, -1.0f);
            static constexpr Vec3Scalar up(0.0f, 1.0f, 0.0f);

            const Mat4Scalar res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
    }
}
