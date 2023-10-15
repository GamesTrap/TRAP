#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    void RunDecomposeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tVec3<T> expectedTranslation(10.0f, 20.0f, 30.0f);
            const TRAP::Math::tVec3<T> expectedRotation(0.0f, 0.0f, TRAP::Math::ATan(0.5f, 0.866f));
            const TRAP::Math::tQuat<T> expectedRotationQuat(TRAP::Math::Radians(expectedRotation));
            static constexpr TRAP::Math::tVec3<T> expectedScale(1.0f, 1.0f, 1.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(expectedRotation))) * TRAP::Math::Scale(expectedScale);

            TRAP::Math::tVec3<T> translation{};
            TRAP::Math::tVec3<T> rotation{};
            TRAP::Math::tQuat<T> rotationQuat{};
            TRAP::Math::tVec3<T> scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, T(0.0000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), Epsilon)));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> expectedTranslation(5.0f, 10.0f, 15.0f);
            static constexpr TRAP::Math::tVec3<T> expectedRotation(0.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> expectedRotationQuat(TRAP::Math::Radians(expectedRotation));
            static constexpr TRAP::Math::tVec3<T> expectedScale(2.0f, 3.0f, 4.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(expectedRotation))) * TRAP::Math::Scale(expectedScale);

            TRAP::Math::tVec3<T> translation{};
            TRAP::Math::tVec3<T> rotation{};
            TRAP::Math::tQuat<T> rotationQuat{};
            TRAP::Math::tVec3<T> scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, T(0.0000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), Epsilon)));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> expectedTranslation(10.0f, 20.0f, 30.0f);
            static constexpr TRAP::Math::tVec3<T> expectedRotation(0.0f, 75.0f, 0.0f);
            const TRAP::Math::tQuat<T> expectedRotationQuat(TRAP::Math::Radians(expectedRotation));
            static constexpr TRAP::Math::tVec3<T> expectedScale(-1.0f, -1.0f, -1.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(TRAP::Math::tQuat<T>(TRAP::Math::Radians(expectedRotation))) * TRAP::Math::Scale(expectedScale);

            TRAP::Math::tVec3<T> translation{};
            TRAP::Math::tVec3<T> rotation{};
            TRAP::Math::tQuat<T> rotationQuat{};
            TRAP::Math::tVec3<T> scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, T(0.0001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), T(0.000001f))));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Decompose()", "[math][generic][decompose]")
{
    SECTION("Scalar - double")
    {
        RunDecomposeRunTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunDecomposeRunTimeTests<float>();
    }
}
