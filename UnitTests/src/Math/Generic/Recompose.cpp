#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Recompose()", "[math][generic][recompose][scalar]", f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;
    using Mat4Scalar = TRAP::Math::tMat4<TestType>;
    using QuatScalar = TRAP::Math::tQuat<TestType>;

    static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

    {
        static constexpr Vec3Scalar translation(10.0f, 20.0f, 30.0f);
        static constexpr Vec3Scalar rotation(0.0f, 0.0f, TRAP::Math::Radians(TRAP::Math::ATan(0.5f, 0.866f)));
        static constexpr QuatScalar rotationQuat(rotation);
        static constexpr Vec3Scalar scale(1.0f, 1.0f, 1.0f);

        static constexpr Mat4Scalar expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
    {
        static constexpr Vec3Scalar translation(5.0f, 10.0f, 15.0f);
        static constexpr Vec3Scalar rotation(0.0f, 0.0f, 0.0f);
        static constexpr QuatScalar rotationQuat(rotation);
        static constexpr Vec3Scalar scale(2.0f, 3.0f, 4.0f);

        static constexpr Mat4Scalar expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
    {
        static constexpr Vec3Scalar translation(10.0f, 20.0f, 30.0f);
        static constexpr Vec3Scalar rotation(0.0f, TRAP::Math::Radians(75.0f), 0.0f);
        static constexpr QuatScalar rotationQuat(rotation);
        static constexpr Vec3Scalar scale(-1.0f, -1.0f, -1.0f);

        static constexpr Mat4Scalar expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
}
