#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Core/Base.h"

TEST_CASE("BIT()", "[core][bit]")
{
    STATIC_REQUIRE(BIT(0u) == 1u);
    STATIC_REQUIRE(BIT(1u) == 2u);
    STATIC_REQUIRE(BIT(2u) == 4u);
    STATIC_REQUIRE(BIT(4u) == 16u);
    STATIC_REQUIRE(BIT(8u) == 256u);
    STATIC_REQUIRE(BIT(16u) == 65536u);
    STATIC_REQUIRE(BIT(31u) == 2147483648u);
}
