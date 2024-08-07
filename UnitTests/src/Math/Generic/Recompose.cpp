#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunRecomposeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
            constexpr TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, TRAP::Math::Radians(TRAP::Math::ATan(0.5f, 0.866f)));
            constexpr TRAP::Math::tQuat<T> rotationQuat(rotation);
            constexpr TRAP::Math::tVec3<T> scale(1.0f, 1.0f, 1.0f);

            constexpr TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> translation(5.0f, 10.0f, 15.0f);
            constexpr TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, 0.0f);
            constexpr TRAP::Math::tQuat<T> rotationQuat(rotation);
            constexpr TRAP::Math::tVec3<T> scale(2.0f, 3.0f, 4.0f);

            constexpr TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
            constexpr TRAP::Math::tVec3<T> rotation(0.0f, TRAP::Math::Radians(75.0f), 0.0f);
            constexpr TRAP::Math::tQuat<T> rotationQuat(rotation);
            constexpr TRAP::Math::tVec3<T> scale(-1.0f, -1.0f, -1.0f);

            constexpr TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRecomposeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
            const TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, TRAP::Math::Radians(TRAP::Math::ATan(0.5f, 0.866f)));
            const TRAP::Math::tQuat<T> rotationQuat(rotation);
            static constexpr TRAP::Math::tVec3<T> scale(1.0f, 1.0f, 1.0f);

            const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> translation(5.0f, 10.0f, 15.0f);
            static constexpr TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> rotationQuat(rotation);
            static constexpr TRAP::Math::tVec3<T> scale(2.0f, 3.0f, 4.0f);

            const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
            static constexpr TRAP::Math::tVec3<T> rotation(0.0f, TRAP::Math::Radians(75.0f), 0.0f);
            const TRAP::Math::tQuat<T> rotationQuat(rotation);
            static constexpr TRAP::Math::tVec3<T> scale(-1.0f, -1.0f, -1.0f);

            const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(rotationQuat) * TRAP::Math::Scale(scale);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Recompose()", "[math][generic][recompose]")
{
    SECTION("Scalar - f64")
    {
        RunRecomposeRunTimeTests<f64>();
        RunRecomposeCompileTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunRecomposeRunTimeTests<f32>();
        RunRecomposeCompileTimeTests<f32>();
    }
}
