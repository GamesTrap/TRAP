#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/FileEvent.h"

TEST_CASE("TRAP::Events::FileSystemChangeEvent", "[events][filesystemchangeevent]")
{
    const TRAP::Events::FileSystemChangeEvent fevent = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Modified, "someFile.txt"};

    SECTION("Constructor")
    {
        // const TRAP::Events::FileSystemChangeEvent feventCopyConstructor(fevent); //Deleted

        TRAP::Events::FileSystemChangeEvent fevent1 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Modified, "someFile.txt"};
        const TRAP::Events::FileSystemChangeEvent fevent1MoveConstructor = TRAP::Events::FileSystemChangeEvent(std::move(fevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::FileSystemChangeEvent feventAssignment = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Modified, "someFile.txt"};

        // const TRAP::Events::FileSystemChangeEvent feventCopyAssignment = fevent; //Deleted
        const TRAP::Events::FileSystemChangeEvent feventMoveAssignment = std::move(feventAssignment);
    }

    SECTION("GetStatus()")
    {
        const TRAP::Events::FileSystemChangeEvent fevent1 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Created, "someFile.txt"};
        const TRAP::Events::FileSystemChangeEvent fevent2 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Renamed, "someFile.txt"};
        const TRAP::Events::FileSystemChangeEvent fevent3 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Modified, "someFile.txt"};
        const TRAP::Events::FileSystemChangeEvent fevent4 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Erased, "someFile.txt"};

        REQUIRE(fevent1.GetStatus() == TRAP::FileSystem::FileSystemStatus::Created);
        REQUIRE(fevent2.GetStatus() == TRAP::FileSystem::FileSystemStatus::Renamed);
        REQUIRE(fevent3.GetStatus() == TRAP::FileSystem::FileSystemStatus::Modified);
        REQUIRE(fevent4.GetStatus() == TRAP::FileSystem::FileSystemStatus::Erased);
    }

    SECTION("GetPath()")
    {
        REQUIRE(fevent.GetPath() == "someFile.txt");
    }

    SECTION("GetOldPath()")
    {
        REQUIRE_FALSE(fevent.GetOldPath());

        const TRAP::Events::FileSystemChangeEvent fevent1 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Created, "someFile.txt", "someOldFile.txt"};
        REQUIRE(fevent1.GetOldPath());
        REQUIRE(*fevent1.GetOldPath() == "someOldFile.txt");
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::FileSystemChangeEvent::GetStaticType() == TRAP::Events::EventType::FileChange);
    }

    SECTION("GetEventType()")
    {
        REQUIRE(fevent.GetEventType() == TRAP::Events::EventType::FileChange);
    }

    SECTION("GetName()")
    {
        REQUIRE(fevent.GetName() == "FileSystemChange");
    }

    SECTION("ToString()")
    {
        REQUIRE(fevent.ToString() == fmt::format("FileSystemChangeEvent: Path: {} Status: {}", fevent.GetPath().generic_string(), fevent.GetStatus()));

        const TRAP::Events::FileSystemChangeEvent fevent1 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Created, "someFile.txt", "someOldFile.txt"};
        REQUIRE(fevent1.ToString() == fmt::format("FileSystemChangeEvent: Path: {} Status: {} OldPath: {}", fevent1.GetPath().generic_string(), fevent1.GetStatus(), fevent1.GetOldPath()->generic_string()));
    }

    SECTION("GetCategoryFlags()")
    {
        REQUIRE(fevent.GetCategoryFlags() == TRAP::Events::EventCategory::FileSystemChange);
    }

    SECTION("IsInCategory()")
    {
        REQUIRE(fevent.IsInCategory(TRAP::Events::EventCategory::FileSystemChange));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", fevent) == fmt::format("FileSystemChangeEvent: Path: {} Status: {}", fevent.GetPath(), fevent.GetStatus()));

        const TRAP::Events::FileSystemChangeEvent fevent1 = TRAP::Events::FileSystemChangeEvent{TRAP::FileSystem::FileSystemStatus::Created, "someFile.txt", "someOldFile.txt"};
        REQUIRE(fmt::format("{}", fevent1) == fmt::format("FileSystemChangeEvent: Path: {} Status: {} OldPath: {}", fevent1.GetPath(), fevent1.GetStatus(), fevent1.GetOldPath()->generic_string()));
    }
}
