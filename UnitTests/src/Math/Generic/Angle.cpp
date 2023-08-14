#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunAngleTests()
{
    {
        const TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
    }
    {
        const TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
    }
    if constexpr(std::same_as<T, float>)
    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<T>() - T(1.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
        const T a = TRAP::Math::Angle(q);
        REQUIRE(TRAP::Math::Equal(a, T(1.0f), T(0.000001f)));
    }

    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
        const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
        const T l = TRAP::Math::Length(n);
        REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
        const T a = TRAP::Math::Angle(n);
        REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
    }
    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(0.0f, 1.0f, 1.0f)));
        const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
        const T l = TRAP::Math::Length(n);
        REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
        const T a = TRAP::Math::Angle(n);
        REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
    }
    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(1.0f, 2.0f, 3.0f)));
        const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
        const T l = TRAP::Math::Length(n);
        REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
        const T a = TRAP::Math::Angle(n);
        REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunAngleEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tQuat<T> q(min, min, min, min);
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::IsNaN(a));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(max, max, max, max);
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::IsNaN(a));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(inf, inf, inf, inf);
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::IsNaN(a));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(ninf, ninf, ninf, ninf);
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::IsNaN(a));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(nan, nan, nan, nan);
        const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
        REQUIRE(TRAP::Math::IsNaN(a));
    }
}

TEST_CASE("TRAP::Math::Angle()", "[math][generic][angle]")
{
    SECTION("Quat - double")
    {
        RunAngleTests<double>();
        RunAngleEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunAngleTests<float>();
        RunAngleEdgeTests<float>();
    }
}