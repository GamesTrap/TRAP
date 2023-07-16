#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunSinTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    REQUIRE_THAT(TRAP::Math::Sin(val), Catch::Matchers::WithinRel(std::sin(val), Epsilon));
}

template<typename T>
requires std::floating_point<T>
void RunSinNaNTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    REQUIRE_THAT(TRAP::Math::Sin(val), !Catch::Matchers::WithinRel(std::sin(val), Epsilon));
}

TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin]")
{
    SECTION("NaN - double")
    {
        constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
        RunSinNaNTests(NaN);
    }
    SECTION("NaN - float")
    {
        constexpr float NaN = std::numeric_limits<float>::quiet_NaN();
        RunSinNaNTests(NaN);
    }

    const double val = static_cast<double>(GENERATE(values({-1.5, 0.0, 0.001, 1.001, 1.5, 11.1, 50.0, 150.0})));

    SECTION("Scalar - double")
    {
        RunSinTests(val);
    }
    SECTION("Scalar - float")
    {
        RunSinTests(static_cast<float>(val));
    }
}
