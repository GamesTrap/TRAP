#include "Catch2FmtSupport.h"

#include "TRAP/src/Core/Profiling.h"

TEST_CASE("GetTRAPProfileSystems", "[core][profiling]")
{
    [[maybe_unused]] const ProfileSystems profileSystems = GetTRAPProfileSystems();
}

TEST_CASE("SetTRAPProfileSystems", "[core][profiling]")
{
    //Pre condition: Save current value
    const auto oldValue = GetTRAPProfileSystems();

    SetTRAPProfileSystems(ProfileSystems::All);
    REQUIRE(GetTRAPProfileSystems() == ProfileSystems::All);

    //Post condition: Reset back to old value
    SetTRAPProfileSystems(oldValue);
}
