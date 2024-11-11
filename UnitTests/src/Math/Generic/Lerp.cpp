#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Lerp()", "[math][generic][lerp][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(TestType(5.0f), TestType(15.0f), TestType(0.5f)), TestType(10.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(TestType(2.5f), TestType(7.8f), TestType(0.75f)), TestType(6.475f), TestType(0.000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(TestType(-10.0f), TestType(20.0f), TestType(0.2f)), TestType(-4.0f), TestType(0.0000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(TestType(8.0f), TestType(13.0f), TestType(0.0f)), TestType(8.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(TestType(8.0f), TestType(13.0f), TestType(1.0f)), TestType(13.0f), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Lerp(max, TestType(100.0f), TestType(0.5f)), (max / 2.0f), Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Lerp(nan, TestType(10.0f), TestType(0.3f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Lerp(ninf, inf, TestType(0.6f))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Lerp()", "[math][generic][lerp][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    {
        static constexpr TestType x(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
        static constexpr TestType y(Vec4Scalar(5.0f, 6.0f, 7.0f, 8.0f));
        static constexpr Scalar a = 0.3f;
        static constexpr TestType a2(a);
        static constexpr TestType expected(Vec4Scalar(2.2f, 3.2f, 4.2f, 5.2f));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, TestType(0.000001f))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, TestType(0.000001f))));
    }
    {
        static constexpr TestType x(Vec4Scalar(-1.0f, -2.0f, -3.0f, -4.0f));
        static constexpr TestType y(Vec4Scalar(-5.0f, -6.0f, -7.0f, -8.0f));
        static constexpr Scalar a = 0.5f;
        static constexpr TestType a2(a);
        static constexpr TestType expected(Vec4Scalar(-3.0f, -4.0f, -5.0f, -6.0f));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, Epsilon)));
    }
    {
        static constexpr TestType x(Vec4Scalar(-1.0f,  2.0f, -3.5f,  4.0f));
        static constexpr TestType y(Vec4Scalar( 4.0f, -3.0f,  1.5f, -2.0f));
        static constexpr Scalar a = 0.6f;
        static constexpr TestType a2(a);
        static constexpr TestType expected(Vec4Scalar(2.0f, -1.0f, -0.5f, 0.4f));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, TestType(0.000001f))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, TestType(0.000001f))));
    }
    {
        static constexpr TestType x(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr TestType y(Vec4Scalar(5.0f, 3.0f, 10.0f, 2.0f));
        static constexpr Scalar a = 0.8f;
        static constexpr TestType a2(a);
        static constexpr TestType expected(Vec4Scalar(4.0f, 2.4f, 8.0f, 1.6f));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, TestType(0.000001f))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, TestType(0.000001f))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Lerp()", "[math][generic][lerp][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    {
        static constexpr TestType q1(1.0f, 2.0f, 3.0f, 4.0f);
        static constexpr TestType q2(5.0f, 6.0f, 7.0f, 8.0f);
        static constexpr TestType res(3.0f, 4.0f, 5.0f, 6.0f);
        static constexpr Scalar a(0.5f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, Epsilon)));
    }
    {
        static constexpr TestType q(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr Scalar a(0.3f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q, q, a), q, Epsilon)));
    }
    {
        static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TestType q2(0.5f, 0.5f, 0.5f, 0.5f);
        static constexpr TestType res(0.65f, 0.35f, 0.35f, 0.35f);
        static constexpr Scalar a(0.7f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, Scalar(0.0000001f))));
    }
    {
        static constexpr TestType q1(-1.0f, -2.0f, -3.0f, -4.0f);
        static constexpr TestType q2(-5.0f, -6.0f, -7.0f, -8.0f);
        static constexpr TestType res(-2.0f, -3.0f, -4.0f, -5.0f);
        static constexpr Scalar a(0.25f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, Epsilon)));
    }
}
