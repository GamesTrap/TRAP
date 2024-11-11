#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Reflect()", "[math][generic][reflect][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 0.0f, -1.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr TestType expected(Vec4Scalar(0.0f, 0.0f, -1.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType expected(Vec4Scalar(1.0f, -1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            static constexpr TestType incident(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr TestType normal(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType incident(Vec4Scalar(max, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(max, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(min, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(min, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(inf, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(inf, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(ninf, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(ninf, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr TestType incident(Vec4Scalar(nan, 0.0f, 0.0f, 0.0f));
            static constexpr TestType normal(Vec4Scalar(nan, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
    }
}
