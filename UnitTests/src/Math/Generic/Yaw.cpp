#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunYawCompileTimeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr T expected = 0.0f;
        constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        constexpr T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        static_assert(TRAP::Math::Equal(yaw, expected, Epsilon));
    }
    {
        constexpr T expected = 90.0f;
        constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        constexpr T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        static_assert(TRAP::Math::Equal(yaw, expected, T(0.1f)));
    }
    {
        constexpr T expected = 45.0f;
        constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        constexpr T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        static_assert(TRAP::Math::Equal(yaw, expected, T(0.0000000000001f)));
    }
    {
        constexpr T expected = 180.0f;
        constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        constexpr T yaw = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Yaw(q)));
        static_assert(TRAP::Math::Equal(yaw, T(0.0f), Epsilon));
    }
    {
        constexpr T expected = -45.0f;
        constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        constexpr T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        static_assert(TRAP::Math::Equal(yaw, expected, T(0.0000000000001f)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunYawRunTimeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        const T expected = 0.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::Equal(yaw, expected, Epsilon));
    }
    {
        const T expected = 90.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::Equal(yaw, expected, T(0.0000000000001f)));
    }
    {
        const T expected = 45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::Equal(yaw, expected, T(0.00001f)));
    }
    {
        const T expected = 180.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Yaw(q)));
        REQUIRE(TRAP::Math::Equal(yaw, expected, Epsilon));
    }
    {
        const T expected = -45.0f;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::Equal(yaw, expected, T(0.00001f)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunYawEdgeTests()
{
    static constexpr T inf = std::numeric_limits<T>::infinity();
    static constexpr T ninf = -std::numeric_limits<T>::infinity();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const T expected = inf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::IsNaN(yaw));
    }
    {
        const T expected = ninf;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::IsNaN(yaw));
    }
    {
        const T expected = nan;
        const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, expected, 0.0f)));
        const T yaw = TRAP::Math::Degrees(TRAP::Math::Yaw(q));
        REQUIRE(TRAP::Math::IsNaN(yaw));
    }
}

TEST_CASE("TRAP::Math::Yaw()", "[math][generic][yaw]")
{
    SECTION("Quat - double")
    {
        RunYawRunTimeTests<double>();
        RunYawCompileTimeTests<double>();
        RunYawEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunYawRunTimeTests<float>();
        RunYawCompileTimeTests<float>();
        RunYawEdgeTests<float>();
    }
}