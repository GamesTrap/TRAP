#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunRecomposeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        static constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
        const TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, TRAP::Math::ATan(0.5f, 0.866f));
        const TRAP::Math::tQuat<T> rotationQuat(TRAP::Math::Radians(rotation));
        static constexpr TRAP::Math::tVec3<T> scale(1.0f, 1.0f, 1.0f);

        const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(rotation))) * TRAP::Math::Scale(scale);

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> translation(5.0f, 10.0f, 15.0f);
        static constexpr TRAP::Math::tVec3<T> rotation(0.0f, 0.0f, 0.0f);
        const TRAP::Math::tQuat<T> rotationQuat(TRAP::Math::Radians(rotation));
        static constexpr TRAP::Math::tVec3<T> scale(2.0f, 3.0f, 4.0f);

        const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(rotation))) * TRAP::Math::Scale(scale);

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> translation(10.0f, 20.0f, 30.0f);
        static constexpr TRAP::Math::tVec3<T> rotation(0.0f, 75.0f, 0.0f);
        const TRAP::Math::tQuat<T> rotationQuat(TRAP::Math::Radians(rotation));
        static constexpr TRAP::Math::tVec3<T> scale(-1.0f, -1.0f, -1.0f);

        const TRAP::Math::tMat4<T> expectedMatrix = TRAP::Math::Translate(translation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(rotation))) * TRAP::Math::Scale(scale);

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotation, scale), expectedMatrix, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Recompose(translation, rotationQuat, scale), expectedMatrix, Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Recompose()", "[math][generic][recompose]")
{
    SECTION("Scalar - double")
    {
        RunRecomposeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunRecomposeTests<float>();
    }
}
