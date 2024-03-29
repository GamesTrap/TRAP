#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunEulerAnglesCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr TRAP::Math::tQuat<T> q(1.0f, 0.0f, 0.0f, 1.0f);
            constexpr T roll = TRAP::Math::Roll(q);
            constexpr T pitch = TRAP::Math::Pitch(q);
            constexpr T yaw = TRAP::Math::Yaw(q);
            constexpr TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 1.0f);
            constexpr T roll = TRAP::Math::Roll(q);
            constexpr T pitch = TRAP::Math::Pitch(q);
            constexpr T yaw = TRAP::Math::Yaw(q);
            constexpr TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 0.5f);
            constexpr T roll = TRAP::Math::Roll(q);
            constexpr T pitch = TRAP::Math::Pitch(q);
            constexpr T yaw = TRAP::Math::Yaw(q);
            constexpr TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunEulerAnglesRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tQuat<T> q(1.0f, 0.0f, 0.0f, 1.0f);
            const T roll = TRAP::Math::Roll(q);
            const T pitch = TRAP::Math::Pitch(q);
            const T yaw = TRAP::Math::Yaw(q);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 1.0f);
            const T roll = TRAP::Math::Roll(q);
            const T pitch = TRAP::Math::Pitch(q);
            const T yaw = TRAP::Math::Yaw(q);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 0.5f);
            const T roll = TRAP::Math::Roll(q);
            const T pitch = TRAP::Math::Pitch(q);
            const T yaw = TRAP::Math::Yaw(q);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunEulerAnglesEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            static constexpr TRAP::Math::tQuat<T> q(min, min, 1.0f, 0.5f);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(max, max, 1.0f, 0.5f);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(inf, inf, 1.0f, 0.5f);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(ninf, ninf, 1.0f, 0.5f);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(nan, nan, 1.0f, 0.5f);
            const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
        }
    }
}

TEST_CASE("TRAP::Math::EulerAngles()", "[math][generic][eulerangles]")
{
    SECTION("Quat - f64")
    {
        RunEulerAnglesRunTimeTests<f64>();
        RunEulerAnglesCompileTimeTests<f64>();
        RunEulerAnglesEdgeTests<f64>();
    }
    SECTION("Quat - f32")
    {
        RunEulerAnglesRunTimeTests<f32>();
        RunEulerAnglesCompileTimeTests<f32>();
        RunEulerAnglesEdgeTests<f32>();
    }
}