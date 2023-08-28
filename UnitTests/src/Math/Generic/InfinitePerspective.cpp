#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunInfinitePerspectiveTests()
{
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(TRAP::Math::Radians<T>(45.0f), 1.0f, 0.1f);
        constexpr TRAP::Math::tMat4<T> expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -0.2f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(TRAP::Math::Radians<T>(90.0f), 16.0f / 9.0f, 0.01f);
        constexpr TRAP::Math::tMat4<T> expected(0.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -0.02f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(TRAP::Math::Radians<T>(30.0f), 2.0f, 1.0f);
        constexpr TRAP::Math::tMat4<T> expected(1.866025f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -2.0f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunInfinitePerspectiveEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const auto p = TRAP::Math::InfinitePerspective<T>(min, min, min);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3>(p))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(max, max, max);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3>(p))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(inf, 1.0f, 0.1f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(ninf, 1.0f, 0.1f);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
    }
    {
        const auto p = TRAP::Math::InfinitePerspective<T>(TRAP::Math::Radians(90.0f), 1.0f, nan);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
    }
}

TEST_CASE("TRAP::Math::InfinitePerspective()", "[math][generic][infiniteperspective]")
{
    SECTION("Mat4 - double")
    {
        RunInfinitePerspectiveTests<double>();
        RunInfinitePerspectiveEdgeTests<double>();
    }
    SECTION("Mat4 - float")
    {
        RunInfinitePerspectiveTests<float>();
        RunInfinitePerspectiveEdgeTests<float>();
    }
}