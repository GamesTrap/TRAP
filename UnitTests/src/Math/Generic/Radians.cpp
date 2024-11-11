#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Radians()", "[math][generic][radians][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(0.0f)), TestType(0.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(45.0f)), TestType(0.785398f), TestType(0.000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(90.0f)), TestType(1.5708f), TestType(0.00001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(180.0f)), TestType(3.14159f), TestType(0.00001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(270.0f)), TestType(4.71239f), TestType(0.000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(-45.0f)), TestType(-0.785398f), TestType(0.000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(-90.0f)), TestType(-1.5708f), TestType(0.00001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(-180.0f)), TestType(-3.14159f), TestType(0.00001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(-270.0f)), TestType(-4.71239f), TestType(0.000001f)));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Radians(TestType(360.0f)), TestType(6.28319f), TestType(0.00001f)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        STATIC_REQUIRE(TRAP::Math::IsInf(inf));
        STATIC_REQUIRE(TRAP::Math::IsInf(ninf));
        STATIC_REQUIRE(TRAP::Math::IsNaN(nan));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Radians()", "[math][generic][radians][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases")
    {
        {
            static constexpr TestType degrees(Vec4Scalar(0.0f, 45.0f, 90.0f, 180.0f));
            static constexpr TestType expected(Vec4Scalar(0.0f, 0.785398f, 1.5708f, 3.14159f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, Scalar(0.00001f))));
        }
        {
            static constexpr TestType degrees(Vec4Scalar(270.0f, -45.0f, -90.0f, -180.0f));
            static constexpr TestType expected(Vec4Scalar(4.71239f, -0.785398f, -1.5708f, -3.14159f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, Scalar(0.00001f))));
        }
        {
            static constexpr TestType degrees(Vec4Scalar(-270.0f, 360.0f, 85.0f, -99.0f));
            static constexpr TestType expected(Vec4Scalar(-4.71239f, 6.28319f, 1.48353f, -1.72788f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, Scalar(0.00001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Radians(TestType(inf)))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Radians(TestType(ninf)))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Radians(TestType(nan)))));
        }
    }
}
