#include <catch2/catch_test_macros.hpp>

#include "Core/PlatformDetection.h"
#include "Events/FileEvent.h"
#include "FileSystem/FileSystem.h"
#include "TRAP/src/FileSystem/FileSystemWatcher.h"

namespace
{
    std::vector<TRAP::Events::FileSystemChangeEvent> recordedEvents{};
    void CheckForDuplicateEvents()
    {
        for(usize i = 0; i < recordedEvents.size(); ++i)
        {
            const auto& currEvent = recordedEvents[i];
            const auto found = std::ranges::find_if(recordedEvents, [&currEvent](const auto& e)
            {
                return e.GetStatus() == currEvent.GetStatus() &&
                       e.GetPath() == currEvent.GetPath() &&
                       e.GetOldPath() == currEvent.GetOldPath();
            });
            const bool isSameIndex = i == std::distance(recordedEvents.begin(), found);
            if(!isSameIndex)
                REQUIRE(found == std::ranges::end(recordedEvents));
        }
    }

    void FileSystemChangeEventCallback(TRAP::Events::Event& event)
    {
        TRAP::Events::FileSystemChangeEvent* const fsEvent = dynamic_cast<TRAP::Events::FileSystemChangeEvent* const>(&event);
        REQUIRE(fsEvent);

        recordedEvents.push_back(std::move(*fsEvent));

        CheckForDuplicateEvents();
    }

    void TestSetEventCallback(TRAP::FileSystem::FileSystemWatcher& fsWatcher)
    {
        fsWatcher.SetEventCallback(::FileSystemChangeEventCallback);

        REQUIRE(fsWatcher.GetEventCallback().target<decltype(&::FileSystemChangeEventCallback)>());
    }

    void TestGetEventCallback(TRAP::FileSystem::FileSystemWatcher& fsWatcher)
    {
        REQUIRE(!fsWatcher.GetEventCallback());

        fsWatcher.SetEventCallback(::FileSystemChangeEventCallback);

        REQUIRE(fsWatcher.GetEventCallback().target<decltype(&::FileSystemChangeEventCallback)>());
    }

    void TestAddFolder(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath, const bool recursive)
    {
        const std::array<std::filesystem::path, 3> folders
        {
            basePath,
            basePath / "a",
            basePath / "a/1"
        };
        for(const auto& p : folders)
        {
            TRAP::FileSystem::CreateFolder(p);
            fsWatcher.AddFolder(p).wait();
        }
        fsWatcher.AddFolder("").wait();

        const auto fsFolders = fsWatcher.GetFolders();
        if(recursive)
        {
            REQUIRE(fsFolders.size() == 1);
            REQUIRE(fsFolders[0] == *TRAP::FileSystem::ToAbsolutePath(folders[0]));
        }
        else
        {
            REQUIRE(fsFolders.size() == folders.size());
            for(usize i = 0; i < fsFolders.size(); ++i)
                REQUIRE(fsFolders[i] == *TRAP::FileSystem::ToAbsolutePath(folders[i]));
        }

        TRAP::FileSystem::Delete(basePath);
    }

    void TestAddFolders(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath, const bool recursive)
    {
        const std::vector<std::filesystem::path> paths
        {
            basePath,
            basePath / "a",
            basePath / "a/1"
        };
        for(const auto& p : paths)
            TRAP::FileSystem::CreateFolder(p);

        fsWatcher.AddFolders(paths).wait();
        fsWatcher.AddFolders({}).wait();

        auto fsFolders = fsWatcher.GetFolders();
        if(recursive)
        {
            REQUIRE(fsFolders.size() == 1);
            REQUIRE(fsFolders[0] == *TRAP::FileSystem::ToAbsolutePath(paths[0]));
        }
        else
        {
            REQUIRE(fsFolders.size() == paths.size());
            for(usize i = 0; i < fsFolders.size(); ++i)
                REQUIRE(fsFolders[i] == *TRAP::FileSystem::ToAbsolutePath(paths[i]));
        }

        const std::vector<std::filesystem::path> extraPaths
        {
            basePath,
            basePath / "a",
            basePath / "nonexisting"
        };
        fsWatcher.AddFolders(extraPaths).wait();

        fsFolders = fsWatcher.GetFolders();
        if(recursive)
            REQUIRE(fsFolders.size() == 1);
        else
            REQUIRE(fsFolders.size() == paths.size());

        TRAP::FileSystem::Delete(basePath);
    }

    void TestRemoveFolder(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath, const bool recursive)
    {
        const std::vector<std::filesystem::path> paths
        {
            basePath,
            basePath / "a",
        };
        for(const auto& p : paths)
            TRAP::FileSystem::CreateFolder(p);
        fsWatcher.AddFolders(paths).wait();
        if(recursive)
            REQUIRE(fsWatcher.GetFolders().size() == 1);
        else
            REQUIRE(fsWatcher.GetFolders().size() == paths.size());

        if(recursive)
            fsWatcher.RemoveFolder(paths[0]);
        else
            fsWatcher.RemoveFolder(paths[1]);
        fsWatcher.RemoveFolder("");
        if(recursive)
            REQUIRE(fsWatcher.GetFolders().empty());
        else
            REQUIRE(fsWatcher.GetFolders() == std::vector{*TRAP::FileSystem::ToAbsolutePath(paths[0])});

        fsWatcher.RemoveFolder("nonexisting");
        if(recursive)
            REQUIRE(fsWatcher.GetFolders().empty());
        else
            REQUIRE(fsWatcher.GetFolders() == std::vector{*TRAP::FileSystem::ToAbsolutePath(paths[0])});

        TRAP::FileSystem::Delete(basePath);
    }

    void TestRemoveFolders(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath, const bool recursive)
    {
        const std::vector<std::filesystem::path> paths
        {
            basePath,
            basePath / "a",
            basePath / "a/b",
            basePath / "b",
        };
        for(const auto& p : paths)
            TRAP::FileSystem::CreateFolder(p);
        fsWatcher.AddFolders(paths).wait();
        if(recursive)
            REQUIRE(fsWatcher.GetFolders().size() == 1);
        else
            REQUIRE(fsWatcher.GetFolders().size() == paths.size());

        fsWatcher.RemoveFolders(std::vector<std::filesystem::path>
            {
                *TRAP::FileSystem::ToAbsolutePath(basePath),
                *TRAP::FileSystem::ToAbsolutePath(basePath / "a"),
                *TRAP::FileSystem::ToAbsolutePath(basePath / "a/b"),
                *TRAP::FileSystem::ToAbsolutePath(basePath / "nonexisting")
            });
        fsWatcher.RemoveFolders({});
        if(recursive)
            REQUIRE(fsWatcher.GetFolders().empty());
        else
            REQUIRE(fsWatcher.GetFolders() == std::vector{*TRAP::FileSystem::ToAbsolutePath(paths[3])});

        TRAP::FileSystem::Delete(basePath);
    }

    void TestGetFolders(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath)
    {
        TRAP::FileSystem::CreateFolder(basePath);
        fsWatcher.AddFolder(basePath).wait();
        REQUIRE(fsWatcher.GetFolders().size() == 1);
        REQUIRE(fsWatcher.GetFolders()[0] == *TRAP::FileSystem::ToAbsolutePath(basePath));

        TRAP::FileSystem::Delete(basePath);
    }

    void TestEvents(TRAP::FileSystem::FileSystemWatcher& fsWatcher, const std::filesystem::path& basePath)
    {
        TRAP::FileSystem::CreateFolder(basePath);
        TRAP::FileSystem::CreateFolder(basePath / "a");

        fsWatcher.SetEventCallback(FileSystemChangeEventCallback);
        fsWatcher.AddFolders({basePath, basePath / "a"}).wait();

        TRAP::FileSystem::WriteTextFile(basePath / "a/test.txt", "Hello World!");
        TRAP::FileSystem::Rename(basePath / "a/test.txt", "test_new.txt");
        TRAP::FileSystem::Move(basePath / "a/test_new.txt", basePath / "test_moved.txt");
        TRAP::FileSystem::Move(basePath / "test_moved.txt", basePath / "../test_moved.txt");
        TRAP::FileSystem::Move(basePath / "../test_moved.txt", basePath / "test_moved_1.txt");
        TRAP::FileSystem::Delete(basePath / "a");
        TRAP::FileSystem::Delete(basePath / "test_moved_1.txt");

        TRAP::FileSystem::Delete(basePath);
        fsWatcher.SetEventCallback({});
        recordedEvents.clear();
    }
}

TEST_CASE("TRAP::FileSystem::FileSystemWatcher", "[filesystem][filesystemwatcher][nonrecursive]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    const std::filesystem::path BasePath = "Testfiles/FileSystemWatcher";
    const bool Recursive = false;
    TRAP::FileSystem::FileSystemWatcher fsWatcher(false, "UnitTest Watcher (non recursive)");

    SECTION("SetEventCallback()")
    {
        TestSetEventCallback(fsWatcher);
    }

    SECTION("GetEventCallback()")
    {
        TestGetEventCallback(fsWatcher);
    }

    SECTION("AddFolder()")
    {
        TestAddFolder(fsWatcher, BasePath, Recursive);
    }

    SECTION("AddFolders()")
    {
        TestAddFolders(fsWatcher, BasePath, Recursive);
    }

    SECTION("RemoveFolder()")
    {
        TestRemoveFolder(fsWatcher, BasePath, Recursive);
    }

    SECTION("RemoveFolders()")
    {
        TestRemoveFolders(fsWatcher, BasePath, Recursive);
    }

    SECTION("GetFolders()")
    {
        TestGetFolders(fsWatcher, BasePath);
    }

    SECTION("Events")
    {
        TestEvents(fsWatcher, BasePath);
    }
}

TEST_CASE("TRAP::FileSystem::FileSystemWatcher - Recursive", "[filesystem][filesystemwatcher][recursive]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    const std::filesystem::path BasePath = "Testfiles/FileSystemWatcher_Recursive";
    const bool Recursive = true;
    TRAP::FileSystem::FileSystemWatcher fsWatcher(true, "UnitTest Watcher (recursive)");

    SECTION("SetEventCallback()")
    {
        TestSetEventCallback(fsWatcher);
    }

    SECTION("GetEventCallback()")
    {
        TestGetEventCallback(fsWatcher);
    }

    SECTION("AddFolder()")
    {
        TestAddFolder(fsWatcher, BasePath, Recursive);
    }

    SECTION("AddFolders()")
    {
        TestAddFolders(fsWatcher, BasePath, Recursive);
    }

    SECTION("RemoveFolder()")
    {
        TestRemoveFolder(fsWatcher, BasePath, Recursive);
    }

    SECTION("RemoveFolders()")
    {
        TestRemoveFolders(fsWatcher, BasePath, Recursive);
    }

    SECTION("GetFolders()")
    {
        TestGetFolders(fsWatcher, BasePath);
    }

    SECTION("Events")
    {
        TestEvents(fsWatcher, BasePath);
    }
}
