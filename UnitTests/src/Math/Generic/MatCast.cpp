#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Mat3Cast()", "[math][generic][mat3cast][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Mat3Scalar = TRAP::Math::tMat3<Scalar>;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            static constexpr Mat3Scalar expected(1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType q1 = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            static constexpr Mat3Scalar expected = TRAP::Math::Rotate(Mat4Scalar(Scalar(1.0f)), TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType q1(0.7071f, 0.0f, 0.0f, 0.7071f);
            static constexpr Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            static constexpr Mat3Scalar expected(0.000019f, 0.999981f, 0.0f, -0.999981f, 0.000019f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
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
            static constexpr TestType q1(min, min, min, min);
            const Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(max, max, max, max);
            const Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(inf, inf, inf, inf);
            const Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(ninf, ninf, ninf, ninf);
            const Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(nan, nan, nan, nan);
            const Mat3Scalar res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Mat4Cast()", "[math][generic][mat4cast][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            static constexpr Mat4Scalar expected(1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType q1 = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            static constexpr Mat4Scalar expected = TRAP::Math::Rotate(Mat4Scalar(Scalar(1.0f)), TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TestType q1(0.7071f, 0.0f, 0.0f, 0.7071f);
            static constexpr Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            static constexpr Mat4Scalar expected(0.000019f, 0.999981f, 0.0f, 0.0f, -0.999981f, 0.000019f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Scalar(0.000001f))));
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
            static constexpr TestType q1(min, min, min, min);
            const Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(max, max, max, max);
            const Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(inf, inf, inf, inf);
            const Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(ninf, ninf, ninf, ninf);
            const Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr TestType q1(nan, nan, nan, nan);
            const Mat4Scalar res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
    }
}
