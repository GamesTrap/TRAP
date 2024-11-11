#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Length()", "[math][generic][length][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        if constexpr(std::same_as<TestType, TRAP::Math::tVec2<Scalar>>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f)), Scalar(3.6055512754639891f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f)), Scalar(10.295630140987001f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec3<Scalar>>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f, 6.0f)), Scalar(7.0f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f, -12.0f)), Scalar(15.811388300841896f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f, 6.0f, 8.0f)), Scalar(10.63014581273465f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f, -12.0f, -15.0f)), Scalar(21.794494717703369f), Scalar(0.000001f)));
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
    }

    SECTION("Normal cases - std")
    {
        if constexpr(std::same_as<TestType, TRAP::Math::tVec2<Scalar>>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f)), Scalar(3.6055512754639891f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f)), Scalar(10.295630140987001f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec3<Scalar>>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f, 6.0f)), Scalar(7.0f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f, -12.0f)), Scalar(15.811388300841896f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(2.0f, 3.0f, 6.0f, 8.0f)), Scalar(10.63014581273465f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(-5.0f, -9.0f, -12.0f, -15.0f)), Scalar(21.794494717703369f), Scalar(0.000001f)));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f, 0.0f)), Scalar(0.0f), Scalar(0.000001f)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(TestType(Vec4Scalar(nan, 1.0f, 2.0f, 3.0f)))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(TestType(Vec4Scalar(inf, 4.0f, 6.0f, 8.0f)))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(TestType(Vec4Scalar(ninf, 4.0f, 6.0f, 8.0f)))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Length()", "[math][generic][length][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(1.0f, 2.0f, 3.0f, 4.0f)), Scalar(5.4772255750516612f), Scalar(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(-2.0f, -3.0f, -4.0f, -5.0f)), Scalar(7.3484692283495345f), Scalar(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f, 0.0f)), Scalar(0.0f), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(1.0f, 2.0f, 3.0f, 4.0f)), Scalar(5.4772255750516612f), Scalar(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(-2.0f, -3.0f, -4.0f, -5.0f)), Scalar(7.3484692283495345f), Scalar(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(TestType(0.0f, 0.0f, 0.0f, 0.0f)), Scalar(0.0f), Epsilon));
    }
}
