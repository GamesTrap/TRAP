#include <catch2/catch_test_macros.hpp>

#include "Utils/Hash/UID.h"

TEST_CASE("TRAP::Utils::UID", "[utils][uid]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::UID>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::UID>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::UID>);
    }

    SECTION("Constructors")
    {
        const TRAP::Utils::UID uid{};

        //Copy constructor
        // const TRAP::Utils::UID uidCopy(uid); //Deleted

        //Move constructor
        TRAP::Utils::UID uidMoveSource{};
        const TRAP::Utils::UID uidMoveTarget(std::move(uidMoveSource));
    }

    SECTION("Assignments")
    {
        //Copy assignment operator
        // const TRAP::Utils::UID uid{};
        // const TRAP::Utils::UID uidCopy = uid; //Deleted

        //Move assignment operator
        TRAP::Utils::UID uidMoveSource{};
        const TRAP::Utils::UID uidMoveTarget = std::move(uidMoveSource);
    }

    SECTION("operator u64()")
    {
        const TRAP::Utils::UID uid{};

        [[maybe_unused]] const u64 uidU64 = uid;
    }

    SECTION("std::hash specialization")
    {
        const TRAP::Utils::UID uid{};

        REQUIRE(std::hash<TRAP::Utils::UID>{}(uid) == u64(uid));
    }
}
