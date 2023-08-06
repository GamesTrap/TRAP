#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeOrthographicTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<T>(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1000.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.001000f, 0.0f, 0.0f, 0.0f, -0.001000f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, T(0.001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<T>(-1.0f, 1.0f, -1.0f, 1.0f, 1000.0f, 0.1f), T(0.001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(0.0f, 800.0f, 0.0f, 600.0f, -10.0f, 10.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.002500f, 0.0f, 0.0f, 0.0f, 0.0f, 0.003333f, 0.0f, 0.0f, 0.0f, 0.0f, -0.050000f, 0.0f, -1.0f, -1.0f, 0.5f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<T>(0.0f, 800.0f, 0.0f, 600.0f, 10.0f, -10.0f), T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(-10.0f, 10.0f, -5.0f, 5.0f, 0.0f, 100.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, -0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, T(0.00000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<T>(-10.0f, 10.0f, -5.0f, 5.0f, 100.0f, 0.0f), T(0.00000001f))));
    }
    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 10.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, -0.111111f, 0.0f, 0.0f, 0.0f, -0.111111f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ<T>(-5.0f, 5.0f, -5.0f, 5.0f, 10.0f, 1.0f), T(0.00000001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunOrthographicEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(min, 1.0f, min, 1.0f, 0.0f, 1.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ(T(min), T(5.0f), T(min), T(5.0f), T(1.0f), T(0.0f)), Epsilon)));
    }
    {
        constexpr TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(max, 1.0f, max, 1.0f, 0.0f, 1.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(ortho, TRAP::Math::OrthographicReverseZ(T(max), T(5.0f), T(max), T(5.0f), T(1.0f), T(0.0f)), Epsilon)));
    }
    {
        const TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(inf, 1.0f, inf, 1.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(ortho[3])));
    }
    {
        const TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(ninf, 1.0f, ninf, 1.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(ortho[3])));
    }
    {
        const TRAP::Math::tMat4<T> ortho = TRAP::Math::Orthographic<T>(nan, 1.0f, nan, 1.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(ortho[3])));
    }
}

TEST_CASE("TRAP::Math::Orthographic()", "[math][generic][orthographic]")
{
    SECTION("Mat4 - double")
    {
        RunCompileTimeOrthographicTests<double>();
        RunOrthographicEdgeTests<double>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeOrthographicTests<float>();
        RunOrthographicEdgeTests<float>();
    }
}