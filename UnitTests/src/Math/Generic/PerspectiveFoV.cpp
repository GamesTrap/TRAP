#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunPerspectiveFoVTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians(45.0f), 800.0f, 600.0f, 0.1f, 100.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.810660f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -0.100100f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians(45.0f), 800.0f, 600.0f, 100.0f, 0.1f), Epsilon)));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians(60.0f), 1024.0f, 768.0f, 0.5f, 200.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.299038f, 0.0f, 0.0f, 0.0f, 0.0f, 1.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.002506f, -1.0f, 0.0f, 0.0f, -0.501253f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians(60.0f), 1024.0f, 768.0f, 200.0f, 0.5f), Epsilon)));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians(90.0f), 640.0f, 480.0f, -1.0f, 1000.0f);
        constexpr TRAP::Math::tMat4<T> expected(0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.999001f, -1.0f, 0.0f, 0.0f, 0.999001f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians(90.0f), 640.0f, 480.0f, 1000.0f, -1.0f), Epsilon)));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians(0.001f), 0.001f, 0.001f, 0.0001f, 0.0002f);
        constexpr TRAP::Math::tMat4<T> expected(114591.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 114591.562500f, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, -1.0f, 0.0f, 0.0f, -0.000200f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.01f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians(0.001f), 0.001f, 0.001f, 0.0002f, 0.0001f), Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunPerspectiveFoVEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians<T>(0.001f), min, min, min, min);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(p[0])));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians<T>(0.001f), min, min, min, min))));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(TRAP::Math::Radians<T>(170.0f), max, max, max, max);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(p[3])));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveFoVReverseZ<T>(TRAP::Math::Radians<T>(170.0f), max, max, max, max))));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(inf, inf, inf, inf, inf);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(p[3])));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(ninf, ninf, ninf, ninf, ninf);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(p[3])));
    }
    {
        const auto p = TRAP::Math::PerspectiveFoV<T>(nan, nan, nan, nan, nan);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(p[3])));
    }
}

TEST_CASE("TRAP::Math::PerspectiveFoV()", "[math][generic][perspectivefov]")
{
    SECTION("Mat4 - double")
    {
        RunPerspectiveFoVTests<double>();
        RunPerspectiveFoVEdgeTests<double>();
    }
    SECTION("Mat4 - float")
    {
        RunPerspectiveFoVTests<float>();
        RunPerspectiveFoVEdgeTests<float>();
    }
}