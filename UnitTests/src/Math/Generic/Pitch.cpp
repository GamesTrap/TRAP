#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunYawTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        const T expected = 0.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::Equal(pitch, expected, Epsilon));
    }
    {
        const T expected = 90.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.0000000000001f)));
    }
    {
        const T expected = 45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
    }
    {
        const T expected = 180.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Pitch(q)));
        REQUIRE(TRAP::Math::Equal(pitch, expected, Epsilon));
    }
    {
        const T expected = -45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunYawEdgeTests()
{
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const T expected = inf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::IsNaN(pitch));
    }
    {
        const T expected = ninf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::IsNaN(pitch));
    }
    {
        const T expected = nan;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
        const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
        REQUIRE(TRAP::Math::IsNaN(pitch));
    }
}

TEST_CASE("TRAP::Math::Pitch()", "[math][generic][pitch]")
{
    SECTION("Quat - double")
    {
        RunYawTests<double>();
        RunYawEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunYawTests<float>();
        RunYawEdgeTests<float>();
    }
}