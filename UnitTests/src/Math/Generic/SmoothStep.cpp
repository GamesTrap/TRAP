#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::SmoothStep()", "[math][generic][smoothstep][scalar]", f32, f64)
{
    static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 0.0f), TestType( 1.0f), TestType( 0.5f)), TestType(0.5f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 2.0f), TestType( 5.0f), TestType( 1.0f)), TestType(0.0f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 0.0f), TestType(10.0f), TestType(15.0f)), TestType(1.0f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(10.0f), TestType(20.0f), TestType(10.0f)), TestType(0.0f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 5.0f), TestType( 8.0f), TestType( 8.0f)), TestType(1.0f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 0.0f), TestType( 1.0f), TestType(-1.0f)), TestType(0.0f), Epsilon));
    STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType( 0.0f), TestType( 1.0f), TestType( 2.0f)), TestType(1.0f), Epsilon));
}

TEMPLATE_TEST_CASE("TRAP::Math::SmoothStep()", "[math][generic][smoothstep][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(0.0f, 0.0f, 0.0f, 1.0f)), TestType(Vec4Scalar(1.0f, 1.0f, 1.0f, 2.0f)), TestType(Vec4Scalar(0.5f, 0.5f, 0.5f, 1.5f))), TestType(0.5f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(1.0f, 1.0f, 1.0f, 1.0f)), TestType(Vec4Scalar(5.0f, 5.0f, 5.0f, 5.0f)), TestType(Vec4Scalar(0.5f, 2.0f, 3.0f, 4.0f))), TestType(Vec4Scalar(0.0f, 0.156250f, 0.5f, 0.843750f)), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f)), TestType(Vec4Scalar(10.0f, 10.0f, 10.0f, 10.0f)), TestType(Vec4Scalar(15.0f, 11.0f, 12.0f, 10.5f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(10.0f, 10.0f, 10.0f, 10.0f)), TestType(Vec4Scalar(20.0f, 20.0f, 20.0f, 20.0f)), TestType(Vec4Scalar(10.0f, 20.0f, 15.0f, 10.0f))), TestType(Vec4Scalar(0.0f, 1.0f, 0.5f, 0.0f)), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f)), TestType(Vec4Scalar(1.0f, 1.0f, 1.0f, 1.0f)), TestType(Vec4Scalar(-1.0f, 2.0f, 3.0f, 0.5f))), TestType(Vec4Scalar(0.0f, 1.0f, 1.0f, 0.5f)), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(TestType(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f)), TestType(Vec4Scalar(1.0f, 1.0f, 1.0f, 1.0f)), TestType(Vec4Scalar(2.0f, 1.0f, 0.0f, 1.5f))), TestType(Vec4Scalar(1.0f, 1.0f, 0.0f, 1.0f)), Epsilon)));
    }
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(1.0f), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(0.0f), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(0.0f), TestType(Vec4Scalar(-1.0f, -2.0f, -3.0f, -4.0f))), TestType(0.0f), Epsilon)));
    }
}
