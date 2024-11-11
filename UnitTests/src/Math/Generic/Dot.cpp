#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Dot()", "[math][generic][dot][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        if constexpr(std::same_as<TestType, TRAP::Math::tVec2<Scalar>>)
        {
            {
                static constexpr TestType x(1.0f, 2.0f);
                static constexpr TestType y(5.0f, 6.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(17.0f), Epsilon));
            }
            {
                static constexpr TestType x(1.0f, 2.0f);
                static constexpr TestType y(2.0f, 4.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(10.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, -2.0f);
                static constexpr TestType y(5.0f, 6.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-17.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, 2.0f);
                static constexpr TestType y(5.0f, -6.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-17.0f), Epsilon));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec3<Scalar>>)
        {
            {
                static constexpr TestType x(1.0f, 2.0f, 3.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(38.0f), Epsilon));
            }
            {
                static constexpr TestType x(1.0f, 2.0f, 3.0f);
                static constexpr TestType y(2.0f, 4.0f, 6.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(28.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, -2.0f, -3.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-38.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, 2.0f, -3.0f);
                static constexpr TestType y(5.0f, -6.0f, 7.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-38.0f), Epsilon));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            {
                static constexpr TestType x(1.0f, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(70.0f), Epsilon));
            }
            {
                static constexpr TestType x(1.0f, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(2.0f, 4.0f, 6.0f, 8.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(60.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, -2.0f, -3.0f, -4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-70.0f), Epsilon));
            }
            {
                static constexpr TestType x(-1.0f, 2.0f, -3.0f, 4.0f);
                static constexpr TestType y(5.0f, -6.0f, 7.0f, -8.0f);
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(-70.0f), Epsilon));
            }
        }

        {
            static constexpr TestType x(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType y(Vec4Scalar(5.0f, 6.0f, 7.0f, 8.0f));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(0.0f), Epsilon));
        }
        {
            static constexpr TestType x(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType y(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(x, y), Scalar(0.0f), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = -std::numeric_limits<Scalar>::quiet_NaN();

        if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            {
                static constexpr TestType x(max, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
            }
            {
                static constexpr TestType x(min, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
            }
            {
                static constexpr TestType x(inf, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
            }
            {
                static constexpr TestType x(ninf, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
            }
            {
                static constexpr TestType x(nan, 2.0f, 3.0f, 4.0f);
                static constexpr TestType y(5.0f, 6.0f, 7.0f, 8.0f);
                REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Dot(x, y)));
            }
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Dot()", "[math][generic][dot][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar Epsilon = Scalar(0.001f);

    {
        static constexpr TestType q1(0.5f, 0.2f, 0.7f, 0.9f);
        static constexpr TestType q2(0.3f, 0.4f, 0.1f, 0.6f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(0.84f), Epsilon));
    }
    {
        static constexpr TestType q1(0.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TestType q2(0.3f, 0.4f, 0.1f, 0.6f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(0.0f), Epsilon));
    }
    {
        static constexpr TestType q1(0.5f, 0.2f, 0.7f, 0.9f);
        static constexpr TestType q2(1.0f, 0.4f, 1.4f, 1.8f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(3.18f), Epsilon));
    }
    {
        static constexpr TestType q1(-0.5f, -0.2f, -0.7f, -0.9f);
        static constexpr TestType q2(0.3f, 0.4f, 0.1f, 0.6f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(-0.84f), Epsilon));
    }
    {
        static constexpr TestType q1(-0.5f, 0.2f, -0.7f, 0.9f);
        static constexpr TestType q2(0.3f, -0.4f, 0.1f, -0.6f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(-0.84f), Epsilon));
    }
    {
        static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TestType q2(0.3f, 0.4f, 0.1f, 0.6f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(0.3f), Epsilon));
    }
    {
        static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TestType q2(1.0f, 0.0f, 0.0f, 0.0f);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), Scalar(1.0f), Epsilon));
    }
}
