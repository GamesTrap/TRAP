#include <catch2/catch_test_macros.hpp>

#include "Utils/Utils.h"

#include <string>
#include <string_view>

namespace
{
    constexpr TRAP::Utils::UUID testUUID{0xABu, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD};
    constexpr std::string_view testUUIDStr = "abcdefab-cdef-abcd-ef01-23456789abcd";
    constexpr std::string_view testUUIDStr1 = "abcdefabcdefabcdef0123456789abcd";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TEST_CASE("TRAP::Utils::UUIDToString()", "[utils][uuid]")
{
    SECTION("Valid")
    {
        STATIC_REQUIRE(TRAP::Utils::UUIDToString(testUUID) == testUUIDStr);
    }

    SECTION("Empty")
    {
        STATIC_REQUIRE(TRAP::Utils::UUIDToString(TRAP::Utils::EMPTY_UUID) == "00000000-0000-0000-0000-000000000000");
    }
}

TEST_CASE("TRAP::Utils::UUIDFromString()", "[utils][uuid]")
{
    SECTION("Valid")
    {
        STATIC_REQUIRE(TRAP::Utils::UUIDFromString(testUUIDStr) == testUUID);
        STATIC_REQUIRE(TRAP::Utils::UUIDFromString(testUUIDStr1) == testUUID);
    }

    SECTION("Invalid")
    {
        STATIC_REQUIRE(TRAP::Utils::UUIDFromString("h0000000000000000000000000000xyz") == TRAP::Utils::EMPTY_UUID);
    }

    SECTION("Empty")
    {
        STATIC_REQUIRE(TRAP::Utils::UUIDFromString("") == TRAP::Utils::EMPTY_UUID);
        STATIC_REQUIRE(TRAP::Utils::UUIDFromString("00000000-0000-0000-0000-000000000000") == TRAP::Utils::EMPTY_UUID);
    }
}
