#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunEulerAnglesTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr TRAP::Math::tQuat<T> q(1.0f, 0.0f, 0.0f, 1.0f);
        const T roll = TRAP::Math::Roll(q);
        const T pitch = TRAP::Math::Pitch(q);
        const T yaw = TRAP::Math::Yaw(q);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 1.0f);
        const T roll = TRAP::Math::Roll(q);
        const T pitch = TRAP::Math::Pitch(q);
        const T yaw = TRAP::Math::Yaw(q);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::tVec3<T>(pitch, yaw, roll), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(1.0f, 1.0f, 1.0f, 0.5f);
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
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tQuat<T> q(min, min, 1.0f, 0.5f);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(max, max, 1.0f, 0.5f);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(inf, inf, 1.0f, 0.5f);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(ninf, ninf, 1.0f, 0.5f);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(nan, nan, 1.0f, 0.5f);
        const TRAP::Math::tVec3<T> angles = TRAP::Math::EulerAngles(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(angles)));
    }
}

TEST_CASE("TRAP::Math::EulerAngles()", "[math][generic][eulerangles]")
{
    SECTION("Quat - double")
    {
        RunEulerAnglesTests<double>();
        RunEulerAnglesEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunEulerAnglesTests<float>();
        RunEulerAnglesEdgeTests<float>();
    }
}