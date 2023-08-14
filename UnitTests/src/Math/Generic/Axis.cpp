#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunAxisTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr TRAP::Math::tQuat<T> q(1.0f, 0.0f, 0.0f, 0.0f);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        constexpr TRAP::Math::tVec3<T> expected(0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Epsilon)));
    }
    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        constexpr TRAP::Math::tVec3<T> expected(0.0f, 1.0f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, Epsilon)));
    }
    {
        const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(-30.0f)), TRAP::Math::tVec3<T>(1.0f, 1.0f, 1.0f));
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        const TRAP::Math::tVec3<T> expected(-1.0f, -1.0f, -1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, expected, T(0.000001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunAxisEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tQuat<T> q(min, min, min, min);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(max, max, max, max);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(inf, inf, inf, inf);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(ninf, ninf, ninf, ninf);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(axis, TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> q(nan, nan, nan, nan);
        const TRAP::Math::tVec3<T> axis = TRAP::Math::Axis(q);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(axis)));
    }
}

TEST_CASE("TRAP::Math::Axis()", "[math][generic][axis]")
{
    SECTION("Quat - double")
    {
        RunAxisTests<double>();
        RunAxisEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunAxisTests<float>();
        RunAxisEdgeTests<float>();
    }
}