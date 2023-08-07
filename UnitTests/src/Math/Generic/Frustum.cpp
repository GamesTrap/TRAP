#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeFrustumTests()
{
    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(-1.0f, 1.0f, -1.0f, 1.0f, 10.0f, 1.0f), T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(-2.0f, 2.0f, -1.0f, 1.0f, 2.0f, 20.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -2.222222f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(-2.0f, 2.0f, -1.0f, 1.0f, 20.0f, 2.0f), T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(-1.5f, 1.5f, -1.5f, 1.5f, 0.5f, 100.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, -1.005025f, -1.0f, 0.0f, 0.0f, -0.502513f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(-1.5f, 1.5f, -1.5f, 1.5f, 100.0f, 0.5f), T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 0.5f);
        constexpr TRAP::Math::tMat4<T> expected(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.25f, -1.0f, 0.0f, 0.0f, -0.125f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 0.1f), T(0.000001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunFrustumEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(min, 1.0f, min, 1.0f, 1.0f, 10.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(min, 1.0f, min, 1.0f, 10.0f, 1.0f), T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(max, 1.0f, max, 1.0f, 1.0f, 10.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -1.111111f, 0.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::FrustumReverseZ<T>(max, 1.0f, max, 1.0f, 10.0f, 1.0f), T(0.000001f))));
    }
    {
        const TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(inf, 1.0f, inf, 1.0f, 1.0f, 10.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(m[2])));
    }
    {
        const TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(ninf, 1.0f, ninf, 1.0f, 1.0f, 10.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(m[2])));
    }
    {
        const TRAP::Math::tMat4<T> m = TRAP::Math::Frustum<T>(nan, 1.0f, nan, 1.0f, 1.0f, 10.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(m[2])));
    }
}

TEST_CASE("TRAP::Math::Frustum()", "[math][generic][frustum]")
{
    SECTION("Mat4 - double")
    {
        RunCompileTimeFrustumTests<double>();
        RunFrustumEdgeTests<double>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeFrustumTests<float>();
        RunFrustumEdgeTests<float>();
    }
}