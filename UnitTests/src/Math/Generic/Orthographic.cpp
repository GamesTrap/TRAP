#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Orthographic()", "[math][generic][orthographic][mat]", TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
            static constexpr TestType expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f), Epsilon)));
        }
        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1000.0f);
            static constexpr TestType expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.001000f, 0.0f, 0.0f, 0.0f, -0.001000f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Scalar(0.001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<Scalar>(-1.0f, 1.0f, -1.0f, 1.0f, 1000.0f, 0.1f), Scalar(0.001f))));
        }
        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(0.0f, 800.0f, 0.0f, 600.0f, -10.0f, 10.0f);
            static constexpr TestType expected(0.002500f, 0.0f, 0.0f, 0.0f, 0.0f, 0.003333f, 0.0f, 0.0f, 0.0f, 0.0f, -0.050000f, 0.0f, -1.0f, -1.0f, 0.5f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<Scalar>(0.0f, 800.0f, 0.0f, 600.0f, 10.0f, -10.0f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(-10.0f, 10.0f, -5.0f, 5.0f, 0.0f, 100.0f);
            static constexpr TestType expected(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, -0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Scalar(0.00000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<Scalar>(-10.0f, 10.0f, -5.0f, 5.0f, 100.0f, 0.0f), Scalar(0.00000001f))));
        }
        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 10.0f);
            static constexpr TestType expected(0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, -0.111111f, 0.0f, 0.0f, 0.0f, -0.111111f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Scalar(0.000001f))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<Scalar>(-5.0f, 5.0f, -5.0f, 5.0f, 10.0f, 1.0f), Scalar(0.00000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(min, 1.0f, min, 1.0f, 0.0f, 1.0f);
            static constexpr TestType expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ(Scalar(min), Scalar(5.0f), Scalar(min), Scalar(5.0f), Scalar(1.0f), Scalar(0.0f)), Epsilon)));
        }
        {
            static constexpr TestType ortho = TRAP::Math::Orthographic<Scalar>(max, 1.0f, max, 1.0f, 0.0f, 1.0f);
            static constexpr TestType expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ(Scalar(max), Scalar(5.0f), Scalar(max), Scalar(5.0f), Scalar(1.0f), Scalar(0.0f)), Epsilon)));
        }
        {
            const TestType ortho = TRAP::Math::Orthographic<Scalar>(inf, 1.0f, inf, 1.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(ortho))));
        }
        {
            const TestType ortho = TRAP::Math::Orthographic<Scalar>(ninf, 1.0f, ninf, 1.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(ortho))));
        }
        {
            const TestType ortho = TRAP::Math::Orthographic<Scalar>(nan, 1.0f, nan, 1.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(ortho))));
        }
    }
}
