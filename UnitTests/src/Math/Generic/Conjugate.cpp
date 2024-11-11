#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Conjugate()", "[math][generic][conjugate][scalar]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<typename TestType::value_type>;

    static constexpr Scalar Epsilon = TRAP::Math::Epsilon<Scalar>();

    {
        static constexpr TestType a(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0));
        STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::NotEqual(a, TRAP::Math::Conjugate(a), Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Conjugate(TRAP::Math::Conjugate(a)), Epsilon)));
    }
    {
        static constexpr TestType input(0.0f, 1.0f, 2.0f, 3.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(0.0f, -1.0f, -2.0f, -3.0f), Epsilon)));
    }
    {
        static constexpr TestType input(2.0f, 0.0f, 0.0f, 0.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(2.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    }
    {
        static constexpr TestType input(1.0f, 2.0f, -3.0f, 4.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(1.0f, -2.0f, 3.0f, -4.0f), Epsilon)));
    }
    {
        const TestType input(1.0f, 2.0f, 3.0f, 4.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(1.0f, -2.0f, -3.0f, -4.0f), Epsilon)));
    }
    {
        static constexpr TestType input(1.0f, 0.0f, 0.0f, 0.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    }
    {
        static constexpr TestType input(-1.0f, -2.0f, -3.0f, -4.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(-1.0f, 2.0f, 3.0f, 4.0f), Epsilon)));
    }

    static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
    static constexpr Scalar max = std::numeric_limits<Scalar>::max();
    static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
    static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
    static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

    {
        static constexpr TestType input(min, -1.0f, 0.0f, 1.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(min, 1.0f, 0.0f, -1.0f), Epsilon)));
    }
    {
        static constexpr TestType input(max, 2.0f, -3.0f, 4.0f);
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Conjugate(input), TestType(max, -2.0f, 3.0f, -4.0f), Epsilon)));
    }
    {
        static constexpr TestType input(inf, 2.0f, -3.0f, 4.0f);
        STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Conjugate(input))));
    }
    {
        static constexpr TestType input(ninf, 2.0f, -3.0f, 4.0f);
        STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Conjugate(input))));
    }
    {
        static constexpr TestType input(nan, 2.0f, -3.0f, 4.0f);
        STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Conjugate(input))));
    }
}
