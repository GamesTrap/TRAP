#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::QuaternionCast()", "[math][generic][quaternioncast][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;
    using QuatScalar = TRAP::Math::tQuat<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType m(1.0f);
            static constexpr QuatScalar res = TRAP::Math::QuaternionCast(m);
            static constexpr QuatScalar expected(1.0f, 0.0f, 0.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType rot(Mat4Scalar(0.866f, -0.5f, 0.0f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
            static constexpr QuatScalar res = TRAP::Math::QuaternionCast(rot);
            static constexpr QuatScalar expected(0.965919f, 0.0f, 0.0f, -0.258821f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
        }

        if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            {
                static constexpr Scalar angle = TRAP::Math::Radians(45.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.923880f, 0.382683f, 0.0f, 0.0f);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
            {
                static constexpr Scalar angle = TRAP::Math::Radians(90.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(0.0f, 1.0f, 0.0f));
                static constexpr QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.707107f, 0.0f, 0.707107f, 0.0f);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
            {
                static constexpr Scalar angle = TRAP::Math::Radians(180.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(0.0f, 0.0f, 1.0f));
                static constexpr QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.0f, 0.0f, 0.0f, 1.0f);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType m(1.0f);
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            static constexpr QuatScalar expected(1.0f, 0.0f, 0.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType rot(Mat4Scalar(0.866f, -0.5f, 0.0f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
            const QuatScalar res = TRAP::Math::QuaternionCast(rot);
            static constexpr QuatScalar expected(0.965919f, 0.0f, 0.0f, -0.258821f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
        }

        if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            {
                static constexpr Scalar angle = TRAP::Math::Radians(45.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.923880f, 0.382683f, 0.0f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
            {
                static constexpr Scalar angle = TRAP::Math::Radians(90.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(0.0f, 1.0f, 0.0f));
                const QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.707107f, 0.0f, 0.707107f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
            {
                static constexpr Scalar angle = TRAP::Math::Radians(180.0f);
                static constexpr TestType rot = TRAP::Math::Rotate(TestType(1.0f), angle, Vec3Scalar(0.0f, 0.0f, 1.0f));
                const QuatScalar res = TRAP::Math::QuaternionCast(rot);
                static constexpr QuatScalar expected(0.0f, 0.0f, 0.0f, 1.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
            }
        }

        if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            const TestType m{Scalar(-1), Scalar(0), Scalar(0), Scalar(2), Scalar(0), Scalar(0), Scalar(0), Scalar(0), Scalar(-3)};
            const QuatScalar res = TRAP::Math::QuaternionCast(m);

            const TestType m1{{Scalar(2.0f), Scalar(1.0f), Scalar(3.0f)}, {Scalar(1.0f), Scalar(6.0f), Scalar(2.0f)}, {Scalar(5.0f), Scalar(3.0f), Scalar(10.0f)}};
            const QuatScalar res1 = TRAP::Math::QuaternionCast(m1);
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
            static constexpr TestType m(Mat4Scalar(min, min, min, min, min, min, min, min, min, min, min, min, min, min, min, min));
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(res)));
        }
        {
            static constexpr TestType m(Mat4Scalar(max, max, max, max, max, max, max, max, max, max, max, max, max, max, max, max));
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(res)));
        }
        {
            static constexpr TestType m(Mat4Scalar(inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf));
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr TestType m(Mat4Scalar(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr TestType m(Mat4Scalar(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            const QuatScalar res = TRAP::Math::QuaternionCast(m);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
    }
}
