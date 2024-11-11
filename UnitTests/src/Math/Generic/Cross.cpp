#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Cross()", "[math][generic][cross][vec]", TRAP::Math::Vec3d, TRAP::Math::Vec3f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = TRAP::Math::Epsilon<Scalar>();

        static constexpr TestType cross1 = TRAP::Math::Cross(TestType(1.0f, 0.0f, 0.0f), TestType(0.0f, 1.0f, 0.0f));
        static constexpr TestType cross2 = TRAP::Math::Cross(TestType(0.0f, 1.0f, 0.0f), TestType(1.0f, 0.0f, 0.0f));
        static constexpr TestType cross3 = TRAP::Math::Cross(TestType(2.0f, 3.0f, 4.0f), TestType(5.0f, 6.0f, 7.0f));
        static constexpr TestType cross4 = TRAP::Math::Cross(TestType(0.0f, 0.0f, 0.0f), TestType(1.0f, 2.0f, 3.0f));
        static constexpr TestType cross5 = TRAP::Math::Cross(TestType(1.0f, 0.0f, 0.0f), TestType(0.0f, 0.0f, 0.0f));
        static constexpr TestType cross6 = TRAP::Math::Cross(TestType(1.0f, 1.0f, 1.0f), TestType(1.0f, 1.0f, 1.0f));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross1, TestType( 0.0f, 0.0f,  1.0f), TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross2, TestType( 0.0f, 0.0f, -1.0f), TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross3, TestType(-3.0f, 6.0f, -3.0f), TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross4, TestType( 0.0f, 0.0f,  0.0f), TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross5, TestType( 0.0f, 0.0f,  0.0f), TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross6, TestType( 0.0f, 0.0f,  0.0f), TestType(Epsilon))));
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        const TestType cross1 = TRAP::Math::Cross(TestType(min, 0.0f, 0.0f), TestType(0.0f, min, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(cross1)));
        const TestType cross2 = TRAP::Math::Cross(TestType(max, 0.0f, 0.0f), TestType(0.0f, max, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(cross2)));
        const TestType cross3 = TRAP::Math::Cross(TestType(inf, 0.0f, 0.0f), TestType(0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross3)));
        const TestType cross4 = TRAP::Math::Cross(TestType(ninf, 0.0f, 0.0f), TestType(0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross4)));
        const TestType cross5 = TRAP::Math::Cross(TestType(0.0f, 0.0f, 0.0f), TestType(0.0f, 0.0f, nan));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross5)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Cross()", "[math][generic][cross][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar Epsilon = TRAP::Math::Epsilon<Scalar>();

    static constexpr TestType cross1 = TRAP::Math::Cross(TestType(1.0f, 0.0f, 0.0f, 0.0f), TestType( 1.0f, 0.0f, 0.0f, 0.0f));
    static constexpr TestType cross2 = TRAP::Math::Cross(TestType(1.0f, 0.0f, 0.0f, 0.0f), TestType( 0.0f, 1.0f, 0.0f, 0.0f));
    static constexpr TestType cross3 = TRAP::Math::Cross(TestType(1.0f, 0.0f, 0.0f, 0.0f), TestType(-1.0f, 0.0f, 0.0f, 0.0f));
    static constexpr TestType cross4 = TRAP::Math::Cross(TestType(0.0f, 0.0f, 0.0f, 0.0f), TestType( 0.1f, 0.2f, 0.3f, 0.4f));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross1, TestType( 1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross2, TestType( 0.0f, 1.0f, 0.0f, 0.0f), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross3, TestType(-1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross4, TestType( 0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));

    TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
    const TestType cross5 = TRAP::Math::Cross(q1, q1);
}
