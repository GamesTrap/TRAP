#include "Catch2FmtSupport.h"

#include "Utils/Map.h"

TEST_CASE("TRAP::Utils::StringHasher", "[utils][map][stringhasher]")
{
    STATIC_REQUIRE(std::same_as<TRAP::Utils::StringHasher::is_transparent, void>);

    REQUIRE(TRAP::Utils::StringHasher{}("Hello World") > 0u);
}

TEST_CASE("TRAP::Utils::UnorderedStringMap", "[utils][map][unorderedstringmap]")
{
    STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::key_type, std::string>);
    STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::mapped_type, u32>);
    STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::hasher, TRAP::Utils::StringHasher>);
    STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::key_equal, std::equal_to<>>);
}
