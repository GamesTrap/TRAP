#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Decompose()", "[math][generic][decompose][scalar]", f64, f32)
{
    using Mat4 = TRAP::Math::tMat4<TestType>;
    using Quat = TRAP::Math::tQuat<TestType>;
    using Vec3 = TRAP::Math::tVec3<TestType>;

    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr Vec3 expectedTranslation(10.0f, 20.0f, 30.0f);
            static constexpr Vec3 expectedRotation(0.0f, 0.0f, TRAP::Math::Radians(TRAP::Math::ATan(0.5f, 0.866f)));
            static constexpr Quat expectedRotationQuat(expectedRotation);
            static constexpr Vec3 expectedScale(1.0f, 1.0f, 1.0f);

            static constexpr Mat4 m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

            Vec3 translation{};
            Vec3 rotation{};
            Quat rotationQuat{};
            Vec3 scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, TestType(0.0000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), Epsilon)));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
        {
            static constexpr Vec3 expectedTranslation(5.0f, 10.0f, 15.0f);
            static constexpr Vec3 expectedRotation(0.0f, 0.0f, 0.0f);
            static constexpr Quat expectedRotationQuat(expectedRotation);
            static constexpr Vec3 expectedScale(2.0f, 3.0f, 4.0f);

            static constexpr Mat4 m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

            Vec3 translation{};
            Vec3 rotation{};
            Quat rotationQuat{};
            Vec3 scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, TestType(0.0000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), Epsilon)));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
        {
            static constexpr Vec3 expectedTranslation(10.0f, 20.0f, 30.0f);
            static constexpr Vec3 expectedRotation(0.0f, TRAP::Math::Radians(75.0f), 0.0f);
            static constexpr Quat expectedRotationQuat(expectedRotation);
            static constexpr Vec3 expectedScale(-1.0f, -1.0f, -1.0f);

            static constexpr Mat4 m = TRAP::Math::Translate(expectedTranslation) * Mat4Cast(expectedRotationQuat) * TRAP::Math::Scale(expectedScale);

            Vec3 translation{};
            Vec3 rotation{};
            Quat rotationQuat{};
            Vec3 scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(translation, expectedTranslation, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotation, expectedRotation, TestType(0.0001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(scale, expectedScale, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotation, scale), TestType(0.000001f))));
            REQUIRE(TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rotationQuat, expectedRotationQuat, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Recompose(translation, rotationQuat, scale), Epsilon)));
        }
        {
            static constexpr Mat4 m
            {
                {1.0f, 0.0f, 0.0f, 0.5f},
                {0.0f, 1.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f}
            };

            Vec3 translation{};
            Vec3 rotation{};
            Vec3 scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
        }
        {
            static constexpr Mat4 m
            {
                {0.0f, 1.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, -1.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f}
            };

            Vec3 translation{};
            Vec3 rotation{};
            Vec3 scale{};

            REQUIRE(TRAP::Math::Decompose(m, translation, rotation, scale));
        }
    }

    SECTION("Edge cases")
    {
        {
            Mat4 m{};

            Vec3 translation{};
            Quat rotationQuat{};
            Vec3 rotationVec{};
            Vec3 scale{};

            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationVec, scale));
        }
        {
            Mat4 m{};
            m[3][3] = TestType(1.0f);

            Vec3 translation{};
            Quat rotationQuat{};
            Vec3 rotationVec{};
            Vec3 scale{};

            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationQuat, scale));
            REQUIRE(!TRAP::Math::Decompose(m, translation, rotationVec, scale));
        }
    }
}
