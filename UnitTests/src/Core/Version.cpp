#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Core/Version.h"

TEST_CASE("TRAP::SemanticVersion", "[core][semanticversion]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::SemanticVersion<1u, 0u, 0u>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::SemanticVersion<1u, 0u, 0u>>);
        STATIC_REQUIRE(std::movable<TRAP::SemanticVersion<1u, 0u, 0u>>);
    }

    SECTION("PreReleaseIdentifier()")
    {
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver1{};
        STATIC_REQUIRE_FALSE(ver1.PreReleaseIdentifier());

        static constexpr std::string_view PreReleaseIdentifier = "preRelease";
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver2{PreReleaseIdentifier};
        STATIC_REQUIRE(ver2.PreReleaseIdentifier());
        STATIC_REQUIRE(*ver2.PreReleaseIdentifier() == PreReleaseIdentifier);

        static constexpr std::string_view MetadataIdentifier = "metadata";
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver3{PreReleaseIdentifier, MetadataIdentifier};
        STATIC_REQUIRE(ver3.PreReleaseIdentifier());
        STATIC_REQUIRE(*ver3.PreReleaseIdentifier() == PreReleaseIdentifier);
    }

    SECTION("MetadataIdentifier()")
    {
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver1{};
        STATIC_REQUIRE_FALSE(ver1.MetadataIdentifier());

        static constexpr std::string_view PreReleaseIdentifier = "preRelease";
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver2{PreReleaseIdentifier};
        STATIC_REQUIRE_FALSE(ver2.MetadataIdentifier());

        static constexpr std::string_view MetadataIdentifier = "metadata";
        static constexpr TRAP::SemanticVersion<1u, 0u, 0u> ver3{PreReleaseIdentifier, MetadataIdentifier};
        STATIC_REQUIRE(ver3.PreReleaseIdentifier());
        STATIC_REQUIRE(*ver3.MetadataIdentifier() == MetadataIdentifier);
    }

    SECTION("format specialization")
    {
        static constexpr TRAP::SemanticVersion<1u, 10u, 3u> ver1{};
        const std::string expected1 = fmt::format("{}.{}.{}", ver1.Major(), ver1.Minor(), ver1.Patch());
        REQUIRE(fmt::format("{}", ver1) == expected1);

        static constexpr std::string_view PreReleaseIdentifier = "preRelease";
        static constexpr TRAP::SemanticVersion<1u, 10u, 3u> ver2{PreReleaseIdentifier};
        const std::string expected2 = fmt::format("{}.{}.{}-{}", ver1.Major(), ver1.Minor(), ver1.Patch(), PreReleaseIdentifier);
        REQUIRE(fmt::format("{}", ver2) == expected2);

        static constexpr std::string_view MetadataIdentifier = "metadata";
        static constexpr TRAP::SemanticVersion<1u, 10u, 3u> ver3{PreReleaseIdentifier, MetadataIdentifier};
        const std::string expected3 = fmt::format("{}.{}.{}-{}+{}", ver1.Major(), ver1.Minor(), ver1.Patch(), PreReleaseIdentifier, MetadataIdentifier);
        REQUIRE(fmt::format("{}", ver3) == expected3);
    }
}
