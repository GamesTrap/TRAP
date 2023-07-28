#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunFMATest()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<std::tuple<T, T, T, T>, 6> values
    {
        std::tuple{T( 2.0f), T( 3.5f), T( 1.25f), T( 8.25f)},
        std::tuple{T(-4.5f), T(-2.0f), T(-1.75f), T( 7.25f)},
        std::tuple{T( 5.0f), T(-1.5f), T( 0.75f), T(-6.75f)},
        std::tuple{T( 0.0f), T( 2.5f), T( 3.0f ), T( 3.0f)},
        std::tuple{T( 2.0f), T( 0.0f), T( 3.0f ), T( 3.0f)},
        std::tuple{T( 2.0f), T( 2.5f), T( 0.0f ), T( 5.0f)},
    };

    for(const auto& [a, b, c, expected] : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::FMA(a, b, c), expected, Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunFMAEdgeTest()
{
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    constexpr std::array<std::tuple<T, T, T>, 7> values
    {
        std::tuple{ max,  max,  max},
        std::tuple{ min,  min,  min},
        std::tuple{-max, -max, -max},
        std::tuple{-min, -min, -min},

        std::tuple{inf, T(3.0f), T(2.5f)},
        std::tuple{T(2.0f), inf, T(3.0f)},
        std::tuple{T(2.0f), T(3.0f), inf},
    };

    for(const auto& [a, b, c] : values)
    {
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::FMA(a, b, c)));
    }

    constexpr std::array<std::tuple<T, T, T>, 3> values2
    {
        std::tuple{T(2.0f), nan, T(3.0f)},
        std::tuple{nan, T(2.5f), T(3.0f)},
        std::tuple{T(2.0f), T(3.0f), nan},
    };

    for(const auto& [a, b, c] : values2)
    {
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMA(a, b, c)));
    }
}

TEST_CASE("TRAP::Math::FMA()", "[math][generic][fma]")
{
    SECTION("Scalar - double")
    {
        RunFMATest<double>();
        RunFMAEdgeTest<double>();
    }
    SECTION("Scalar - float")
    {
        RunFMATest<float>();
        RunFMAEdgeTest<float>();
    }
}