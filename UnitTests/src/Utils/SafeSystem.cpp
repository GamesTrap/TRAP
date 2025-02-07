#include "Catch2FmtSupport.h"

#include "Utils/SafeSystem.h"
#include "Utils/Expected.h"

#ifdef TRAP_PLATFORM_WINDOWS
#include "Utils/Win.h"
#endif /*TRAP_PLATFORM_WINDOWS*/

TEST_CASE("TRAP::Utils::SafeSystem", "[utils][safesystem]")
{
    SECTION("Valid")
    {
#ifdef TRAP_PLATFORM_WINDOWS
        REQUIRE(TRAP::Utils::SafeSystem("cmd /C exit", {}, false));
        REQUIRE(TRAP::Utils::SafeSystem("cmd", {"/C exit"}, false));
        REQUIRE(TRAP::Utils::SafeSystem("cmd /C exit", {}, true));
        REQUIRE(TRAP::Utils::SafeSystem("cmd", {"/C exit"}, true));
#elif defined(TRAP_PLATFORM_LINUX)
        REQUIRE(TRAP::Utils::SafeSystem("echo", {}, false));
        REQUIRE(TRAP::Utils::SafeSystem("sh", {"-c", "exit"}, false));
        REQUIRE(TRAP::Utils::SafeSystem("echo", {}, true));
        REQUIRE(TRAP::Utils::SafeSystem("sh", {"-c", "exit"}, true));
#endif
    }

    SECTION("Invalid")
    {
        REQUIRE(TRAP::Utils::SafeSystem("", {}, false).Error() == "program is empty!");

#ifdef TRAP_PLATFORM_WINDOWS
        REQUIRE_FALSE(TRAP::Utils::SafeSystem(std::string(MAX_PATH + 5, 'F'), {}, false));
        REQUIRE_FALSE(TRAP::Utils::SafeSystem(std::string(MAX_PATH + 5, 'F'), {}, true));
        REQUIRE_FALSE(TRAP::Utils::SafeSystem("lololololol", {std::string(std::numeric_limits<u16>::max(), 'X')}, true));
        REQUIRE_FALSE(TRAP::Utils::SafeSystem("lololololol", {"\"Hello World\""}, false));
        REQUIRE_FALSE(TRAP::Utils::SafeSystem("lololololol", {"\"Hello World\""}, true));
#elif defined(TRAP_PLATFORM_LINUX)
        REQUIRE(TRAP::Utils::SafeSystem("lololololol", {"\"Hello World\""}, false)); //We don't know the success/failure status of the child without waiting for it, so this always returns success.
        REQUIRE(TRAP::Utils::SafeSystem("lololololol", {"\"Hello World\""}, true).Error() == "TRAP::Utils::SafeSystem(): Program exited with non-zero status: 2");
#endif
    }
}
