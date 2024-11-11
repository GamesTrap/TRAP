#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::FMA()", "[math][generic][fma][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::tuple<TestType, TestType, TestType, TestType>, 6u> values
        {
            std::tuple{TestType( 2.0f), TestType( 3.5f), TestType( 1.25f), TestType( 8.25f)},
            std::tuple{TestType(-4.5f), TestType(-2.0f), TestType(-1.75f), TestType( 7.25f)},
            std::tuple{TestType( 5.0f), TestType(-1.5f), TestType( 0.75f), TestType(-6.75f)},
            std::tuple{TestType( 0.0f), TestType( 2.5f), TestType( 3.0f ), TestType( 3.0f)},
            std::tuple{TestType( 2.0f), TestType( 0.0f), TestType( 3.0f ), TestType( 3.0f)},
            std::tuple{TestType( 2.0f), TestType( 2.5f), TestType( 0.0f ), TestType( 5.0f)},
        };

        for(const auto& [a, b, c, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::FMA(a, b, c), expected, Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 7u> values
        {
            std::tuple{ max,  max,  max},
            std::tuple{ min,  min,  min},
            std::tuple{-max, -max, -max},
            std::tuple{-min, -min, -min},

            std::tuple{inf, TestType(3.0f), TestType(2.5f)},
            std::tuple{TestType(2.0f), inf, TestType(3.0f)},
            std::tuple{TestType(2.0f), TestType(3.0f), inf},
        };

        for(const auto& [a, b, c] : values)
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::FMA(a, b, c)));

        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 3u> values2
        {
            std::tuple{TestType(2.0f), nan, TestType(3.0f)},
            std::tuple{nan, TestType(2.5f), TestType(3.0f)},
            std::tuple{TestType(2.0f), TestType(3.0f), nan},
        };

        for(const auto& [a, b, c] : values2)
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMA(a, b, c)));
    }
}
