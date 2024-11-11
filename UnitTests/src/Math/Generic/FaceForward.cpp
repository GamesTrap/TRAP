#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::FaceForward()", "[math][generic][faceforward][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(-2.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(1.0f, 2.0f, 0.0f, 0.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr TestType incident(normal);
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            static constexpr TestType normal(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType incident(Vec4Scalar(2.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType reference(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(TestType(inf), TestType(ninf), TestType(inf)))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(TestType(ninf), TestType(inf), TestType(ninf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::FaceForward(TestType(nan), TestType(nan), TestType(nan)))));
    }
}
