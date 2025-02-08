#include "Catch2FmtSupport.h"
#include <catch2/matchers/catch_matchers_exception.hpp>

#include "Utils/ErrorCodes/ErrorCodes.h"

TEST_CASE("TRAP::Utils::DisplayError", "[utils][displayerror]")
{
    SECTION("Valid")
    {
        REQUIRE_THROWS_MATCHES(TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::FailedToLoadLibrary), std::runtime_error, Catch::Matchers::Message("Failed to load a dynamic library! (0x0021)"));
    }

    SECTION("Invalid")
    {
        REQUIRE_THROWS_MATCHES(TRAP::Utils::DisplayError(static_cast<TRAP::Utils::ErrorCode>(-1)), std::runtime_error, Catch::Matchers::Message("Unknown error occurred!"));
    }
}
