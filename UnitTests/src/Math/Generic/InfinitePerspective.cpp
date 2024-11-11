#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::InfinitePerspectiveReverseZ()", "[math][generic][infiniteperspectivereversez][mat]",
                   TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        {
            static constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(45.0f), 1.0f, 0.1f);
            static constexpr TestType expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.1f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
        }
        {
            static constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(90.0f), 16.0f / 9.0f, 0.01f);
            static constexpr TestType expected(0.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.01f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
        }
        {
            static constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(30.0f), 2.0f, 1.0f);
            static constexpr TestType expected(1.866025f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(45.0f), 1.0f, 0.1f);
            static constexpr TestType expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.1f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(90.0f), 16.0f / 9.0f, 0.01f);
            static constexpr TestType expected(0.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.01f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians<Scalar>(30.0f), 2.0f, 1.0f);
            static constexpr TestType expected(1.866025f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, Scalar(0.000001f))));
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
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(min, min, min);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3u>(p))));
        }
        {
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(max, max, max);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3u>(p))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(inf, 1.0f, 0.1f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(p))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1u>(p))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(ninf, 1.0f, 0.1f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(p))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1u>(p))));
        }
        {
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<Scalar>(TRAP::Math::Radians(90.0f), 1.0f, nan);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1u>(p))));
        }
    }
}
