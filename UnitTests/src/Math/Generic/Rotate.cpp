#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Rotate()", "[math][generic][rotate]", f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;
    using Mat4Scalar = TRAP::Math::tMat4<TestType>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f));
            static constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f));
            static constexpr Mat4Scalar expected(0.333333f, 0.910684f, -0.244017f, 0.0f, -0.244017f, 0.333333f, 0.910684f, 0.0f, 0.910684f, -0.244017f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(45.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(45.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Mat4Scalar expected(0.707107f, 0.0f, -0.707107f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.707107f, 0.0f, 0.707107f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr Mat4Scalar expected(0.5f, 0.866025f, 0.0f, 0.0f, -0.866025f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.5f, 0.5f, 0.5f));
            static constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.5f, 0.5f, 0.5f));
            static constexpr Mat4Scalar expected(0.666667f, 0.666667f, -0.333333f, 0.0f, -0.333333f, 0.666667f, 0.666667f, 0.0f, 0.666667f, -0.333333f, 0.666667f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f));
            static constexpr Mat4Scalar expected(0.333333f, 0.910684f, -0.244017f, 0.0f, -0.244017f, 0.333333f, 0.910684f, 0.0f, 0.910684f, -0.244017f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(45.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(45.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Mat4Scalar expected(0.707107f, 0.0f, -0.707107f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.707107f, 0.0f, 0.707107f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr Mat4Scalar expected(0.5f, 0.866025f, 0.0f, 0.0f, -0.866025f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.5f, 0.5f, 0.5f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(60.0f), Vec3Scalar(0.5f, 0.5f, 0.5f));
            static constexpr Mat4Scalar expected(0.666667f, 0.666667f, -0.333333f, 0.0f, -0.333333f, 0.666667f, 0.666667f, 0.0f, 0.666667f, -0.333333f, 0.666667f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, TestType(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(min, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(min, 1.0f, 1.0f));
            static constexpr Mat4Scalar expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(max, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(max, 1.0f, 1.0f));
            static constexpr Mat4Scalar expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(inf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(inf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t2))));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(ninf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(ninf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t2))));
        }
        {
            const auto t = TRAP::Math::Rotate(Mat4Scalar(1.0f), TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(nan, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<TestType>(90.0f), Vec3Scalar(nan, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0u>(t2))));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Rotate()", "[math][generic][qat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr Vec3Scalar axis(0.0f, 0.0f, 1.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(90.0f));
            static constexpr TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.653281f, 0.270598f, -0.270598f, 0.653281f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(30.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(0.0f, 1.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(45.0f));
            static constexpr TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.793353f, 0.0f, 0.608761f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(60.0f)), Vec3Scalar(1.0f, 1.0f, 1.0f));
            static constexpr Vec3Scalar axis(0.0f, 1.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(-30.0f));
            static constexpr TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.965926f, 0.612372f, 0.258819f, 0.353553f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(90.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(0.0f));
            static constexpr TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.707107f, 0.0f, 0.707107f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(0.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(180.0f));
            static constexpr TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.0f, 1.0f, 0.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(1.0f, 0.0f, 0.0f));
            static constexpr Vec3Scalar axis(0.0f, 0.0f, 1.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(90.0f));
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.653281f, 0.270598f, -0.270598f, 0.653281f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(30.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(0.0f, 1.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(45.0f));
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.793353f, 0.0f, 0.608761f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(60.0f)), Vec3Scalar(1.0f, 1.0f, 1.0f));
            static constexpr Vec3Scalar axis(0.0f, 1.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(-30.0f));
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.965926f, 0.612372f, 0.258819f, 0.353553f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(90.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(0.0f));
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.707107f, 0.0f, 0.707107f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(0.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            static constexpr Scalar angle = TRAP::Math::Radians(Scalar(180.0f));
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
            static constexpr TestType expected(0.0f, 1.0f, 0.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Epsilon)));
        }
        {
            const TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(0.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            const Scalar angle = TRAP::Math::Radians(Scalar(180.0f));
            const Vec3Scalar axis(1.0f, 2.0f, 0.0f);
            const TestType rot = TRAP::Math::Rotate(q, angle, axis);
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

        if constexpr(std::same_as<Scalar, f32>)
        {
            {
                static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(45.0f)), Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr Vec3Scalar axis(0.0f, 1.0f, 0.0f);
                const TestType rot = TRAP::Math::Rotate(q, min, axis);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, TestType(-0.889284074f, -0.368353546f, -0.250453979f, -0.103741445f), Epsilon)));
            }
            {
                static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(30.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
                static constexpr Vec3Scalar axis(0.0f, 0.0f, 1.0f);
                const TestType rot = TRAP::Math::Rotate(q, max, axis);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, TestType(-0.929755926f, 0.0701631084f, -0.249127343f, 0.261852294f), Epsilon)));
            }
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(60.0f)), Vec3Scalar(1.0f, 1.0f, 1.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            const TestType rot = TRAP::Math::Rotate(q, inf, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(90.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            const TestType rot = TRAP::Math::Rotate(q, ninf, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(TRAP::Math::Radians(Scalar(0.0f)), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr Vec3Scalar axis(1.0f, 0.0f, 0.0f);
            const TestType rot = TRAP::Math::Rotate(q, nan, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
    }
}
