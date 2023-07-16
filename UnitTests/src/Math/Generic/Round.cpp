#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunRoundTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    REQUIRE_THAT(TRAP::Math::Round(val), Catch::Matchers::WithinRel(std::round(val), Epsilon));
}

TEST_CASE("TRAP::Math::Round()", "[math][generic][round]")
{
    SECTION("Edge cases")
    {
        constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        constexpr float fmax = std::numeric_limits<float>::max();
        constexpr long double ninf = -std::numeric_limits<long double>::infinity();
        constexpr long double inf = std::numeric_limits<long double>::infinity();
        constexpr long double nan = std::numeric_limits<long double>::quiet_NaN();

        REQUIRE_THAT(TRAP::Math::Round(fmax), Catch::Matchers::WithinRel(std::round(fmax), Epsilon));
        REQUIRE_THAT(TRAP::Math::Round(ninf), Catch::Matchers::WithinRel(std::round(ninf), Epsilon));
        REQUIRE_THAT(TRAP::Math::Round(inf), Catch::Matchers::WithinRel(std::round(inf), Epsilon));
        REQUIRE((TRAP::Math::IsNaN(TRAP::Math::Round(nan)) && std::isnan(std::round(nan))));
    }

    const double val = GENERATE(values(
        {
            0.0, -0.0, 0.1, -0.1, 0.5, -0.5, 0.9, -0.9, 1.0, -1.0,
            1.5, -1.5, 1.9, -1.9, 4.2, 4.5, 4.7, 5.0, -4.2, -4.5,
            -4.7, -5.0, 42e32, -42e32
        }));

    SECTION("Scalar - double")
    {
        RunRoundTests(val);
    }
    SECTION("Scalar - float")
    {
        RunRoundTests(static_cast<float>(val));
    }
}