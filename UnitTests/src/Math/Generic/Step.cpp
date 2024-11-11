#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Step()", "[math][generic][step][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType edge = 2.0f;

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(edge, TestType(1.0f)), TestType(0.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(edge, TestType(3.0f)), TestType(1.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(edge, TestType(2.0f)), TestType(1.0f), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType min = std::numeric_limits<TestType>::denorm_min();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = -std::numeric_limits<TestType>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType( 0.0f), min), TestType(1.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType(-1.0f), min), TestType(1.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType( 1.0f), min), TestType(0.0f), Epsilon));

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType( 0.0f), inf), TestType(1.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType( 0.0f), ninf), TestType(0.0f), Epsilon));

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType(-1.0f), nan), TestType(1.0f), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(TestType( 1.0f), nan), TestType(1.0f), Epsilon));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Step()", "[math][generic][step][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    {
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(TestType(Vec4Scalar(-1.0f, -2.0f, -3.0f, -4.0f)), TestType(Vec4Scalar(-2.0f, -3.0f, -4.0f, -5.0f))), TestType(0.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(TestType(Vec4Scalar( 0.0f,  1.0f,  2.0f,  3.0f)), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(TestType(Vec4Scalar( 2.0f,  3.0f,  4.0f,  5.0f)), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(0.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(TestType(Vec4Scalar( 0.0f,  1.0f,  2.0f,  3.0f)), TestType(Vec4Scalar(-1.0f, -2.0f, -3.0f, -4.0f))), TestType(0.0f), Epsilon)));
    }
    {
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(1.0f), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(0.0f), TestType(Vec4Scalar( 1.0f,  2.0f,  3.0f,  4.0f))), TestType(1.0f), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(Scalar(0.0f), TestType(Vec4Scalar(-1.0f, -2.0f, -3.0f, -4.0f))), TestType(0.0f), Epsilon)));
    }
}
