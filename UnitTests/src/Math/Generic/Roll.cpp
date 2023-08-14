#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunPitchTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        const T expected = 0.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
    }
    {
        const T expected = 90.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::Equal(roll, expected, T(0.0000000000001f)));
    }
    {
        const T expected = 45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::Equal(roll, expected, T(0.00001f)));
    }
    {
        const T expected = 180.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Roll(q)));
        REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
    }
    {
        const T expected = -45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::Equal(roll, expected, T(0.00001f)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunPitchEdgeTests()
{
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const T expected = inf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::IsNaN(roll));
    }
    {
        const T expected = ninf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::IsNaN(roll));
    }
    {
        const T expected = nan;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
        const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
        REQUIRE(TRAP::Math::IsNaN(roll));
    }
}

TEST_CASE("TRAP::Math::Roll()", "[math][generic][roll]")
{
    SECTION("Quat - double")
    {
        RunPitchTests<double>();
        RunPitchEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunPitchTests<float>();
        RunPitchEdgeTests<float>();
    }
}