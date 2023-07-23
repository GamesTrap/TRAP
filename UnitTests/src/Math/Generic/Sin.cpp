#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunSinTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::Sin(val), Catch::Matchers::WithinRel(std::sin(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunSinEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(ninf)));
}

TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin]")
{
    SECTION("Scalar - double")
    {
        RunSinTests<double>();
        RunSinEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSinTests<float>();
        RunSinEdgeTests<float>();
    }
}
