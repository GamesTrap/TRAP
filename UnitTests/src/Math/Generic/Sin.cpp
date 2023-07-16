#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Sin()", "[math][generic]", double, float)
{
    constexpr TestType NaN = std::numeric_limits<TestType>::quiet_NaN();
    constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

    SECTION("NaN")
    {
        REQUIRE_THAT(TRAP::Math::Sin(NaN), !Catch::Matchers::WithinRel(std::sin(NaN), Epsilon));
    }

    SECTION("Scalar")
    {
        const TestType val = static_cast<TestType>(GENERATE(values({-1.5, 0.0, 0.001, 1.001, 1.5, 11.1, 50.0, 150.0})));
        REQUIRE_THAT(TRAP::Math::Sin(val), Catch::Matchers::WithinRel(std::sin(val), Epsilon));
    }
}
