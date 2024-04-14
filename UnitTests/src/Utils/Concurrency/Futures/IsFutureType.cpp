#include <catch2/catch_test_macros.hpp>

#include <future>

#include "Utils/Concurrency/Futures/IsFutureType.h"

TEST_CASE("TRAP::Utils::IsFutureType", "[utils][concurrency][futures][isfuturetype]")
{
    SECTION("Valid")
    {
        STATIC_REQUIRE(TRAP::Utils::IsFutureType<std::future<int>>::value);
        STATIC_REQUIRE(TRAP::Utils::IsFutureType<std::shared_future<int>>::value);
    }

    SECTION("Invalid")
    {
        STATIC_REQUIRE(!TRAP::Utils::IsFutureType<void>::value);
    }
}
