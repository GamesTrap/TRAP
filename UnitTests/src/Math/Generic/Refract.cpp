#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Refract()", "[math][generic][refract][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(-1.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(-4.2838823f, -4.2838823f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Scalar(0.0000001f))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(-1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(-1.0f, -1.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(-1.0f, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(-4.2838823f, -4.2838823f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Scalar(0.0000001f))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(-1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(-1.0f, -1.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType incident(Vec4Scalar(min, min, min, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(max, max, max, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(inf, inf, inf, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(ninf, ninf, ninf, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(nan, nan, nan, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 2.0f, 3.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(nan, nan, nan, 0.0f));
            static constexpr Scalar eta = 1.5f;
            static constexpr TestType expected(Vec4Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }
}
