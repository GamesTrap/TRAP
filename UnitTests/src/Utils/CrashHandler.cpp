#include <catch2/catch_test_macros.hpp>

#include "Utils/CrashHandler/CrashHandler.h"

TEST_CASE("TRAP::Utils::CrashHandler", "[utils][crashhandler]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::CrashHandler>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::CrashHandler>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::CrashHandler>);
    }
}
