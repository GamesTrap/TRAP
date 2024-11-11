#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Frustum()", "[math][generic][frustum][mat]", TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases")
    {
        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
            static constexpr TestType expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 10.0f, 1.0f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(-2.0f, 2.0f, -1.0f, 1.0f, 2.0f, 20.0f);
            static constexpr TestType expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -2.222222f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(-2.0f, 2.0f, -1.0f, 1.0f, 20.0f, 2.0f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(-1.5f, 1.5f, -1.5f, 1.5f, 0.5f, 100.0f);
            static constexpr TestType expected(0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, -1.005025f, -1.0f, 0.0f, 0.0f, -0.502513f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(-1.5f, 1.5f, -1.5f, 1.5f, 100.0f, 0.5f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 0.5f);
            static constexpr TestType expected(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.25f, -1.0f, 0.0f, 0.0f, -0.125f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 0.1f), Scalar(0.000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(min, 1.0f, min, 1.0f, 1.0f, 10.0f);
            static constexpr TestType expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(min, 1.0f, min, 1.0f, 10.0f, 1.0f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType m = TRAP::Math::Frustum<Scalar>(max, 1.0f, max, 1.0f, 1.0f, 10.0f);
            static constexpr TestType expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<Scalar>(max, 1.0f, max, 1.0f, 10.0f, 1.0f), Scalar(0.000001f))));
        }
        {
            const TestType m = TRAP::Math::Frustum<Scalar>(inf, 1.0f, inf, 1.0f, 1.0f, 10.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<2u>(m))));
        }
        {
            const TestType m = TRAP::Math::Frustum<Scalar>(ninf, 1.0f, ninf, 1.0f, 1.0f, 10.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<2u>(m))));
        }
        {
            const TestType m = TRAP::Math::Frustum<Scalar>(nan, 1.0f, nan, 1.0f, 1.0f, 10.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<2u>(m))));
        }
    }
}
