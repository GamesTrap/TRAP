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
            const TRAP::Math::tVec3<T> expectedRotation(0.0f, 0.0f, TRAP::Math::Radians(TRAP::Math::ATan(0.5f, 0.866f)));
            const TRAP::Math::tQuat<T> expectedRotationQuat(expectedRotation);
            static constexpr TRAP::Math::tVec3<T> expectedScale(1.0f, 1.0f, 1.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

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
            const TRAP::Math::tQuat<T> expectedRotationQuat(expectedRotation);
            static constexpr TRAP::Math::tVec3<T> expectedScale(2.0f, 3.0f, 4.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

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
            static constexpr TRAP::Math::tVec3<T> expectedRotation(0.0f, TRAP::Math::Radians(75.0f), 0.0f);
            const TRAP::Math::tQuat<T> expectedRotationQuat(expectedRotation);
            static constexpr TRAP::Math::tVec3<T> expectedScale(-1.0f, -1.0f, -1.0f);

            const TRAP::Math::tMat4<T> m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

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

    template<typename T>
    requires std::floating_point<T>
    void RunDecomposeEdgeRunTimeTests()
    {
        {
            TRAP::Math::tMat4<T> m{};

            TRAP::Math::tVec3<T> translation{};
            TRAP::Math::tQuat<T> rotationQuat{};
            TRAP::Math::tVec3<T> rotationVec{};
            TRAP::Math::tVec3<T> scale{};

            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationVec, scale));
        }
        {
            TRAP::Math::tMat4<T> m{};
            m[3][3] = T(1.0f);

            TRAP::Math::tVec3<T> translation{};
            TRAP::Math::tQuat<T> rotationQuat{};
            TRAP::Math::tVec3<T> rotationVec{};
            TRAP::Math::tVec3<T> scale{};

            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationVec, scale));
        }
    }
}

TEST_CASE("TRAP::Math::Decompose()", "[math][generic][decompose]")
{
    SECTION("Scalar - f64")
    {
        RunDecomposeRunTimeTests<f64>();
        RunDecomposeEdgeRunTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunDecomposeRunTimeTests<f32>();
        RunDecomposeEdgeRunTimeTests<f32>();
    }
}
