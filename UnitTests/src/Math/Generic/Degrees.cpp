#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Degrees()", "[math][generic][degrees][scalar]", f64, f32)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(0.0f))), TestType(0.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(45.0f))), TestType(45.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(90.0f))), TestType(90.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(180.0f))), TestType(180.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(270.0f))), TestType(270.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(-45.0f))), TestType(-45.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(-90.0f))), TestType(-90.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(-180.0f))), TestType(-180.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(-270.0f))), TestType(-270.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(TestType(360.0f))), TestType(360.0f), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        static_assert(TRAP::Math::IsInf(inf));
        static_assert(TRAP::Math::IsInf(ninf));
        static_assert(TRAP::Math::IsNaN(nan));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Degrees()", "[math][generic][degrees][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases")
    {
        {
            static constexpr TestType degrees(Vec4Scalar(0.0f, 45.0f, 90.0f, 180.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, Scalar(0.00001f))));
        }
        {
            static constexpr TestType degrees(Vec4Scalar(270.0f, -45.0f, -90.0f, -180.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, Scalar(0.00001f))));
        }
        {
            static constexpr TestType degrees(Vec4Scalar(-270.0f, 360.0f, 85.0f, -99.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, Scalar(0.00001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Degrees(TestType(inf)))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Degrees(TestType(ninf)))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Degrees(TestType(nan)))));
        }
    }
}
