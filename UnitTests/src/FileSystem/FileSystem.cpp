#include <catch2/catch_test_macros.hpp>

#include <fstream>

#include "Core/PlatformDetection.h"
#include "TRAP/src/FileSystem/FileSystem.h"
#include "TRAP/src/Application.h"

namespace
{
    const std::filesystem::path TestFilesPath = "Testfiles/FileSystem/";
}

TEST_CASE("TRAP::FileSystem::Init()", "[filesystem][init]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    TRAP::Application app{"UnitTest"};

    TRAP::FileSystem::Init();
}

TEST_CASE("TRAP::FileSystem::Shutdown()", "[filesystem][shutdown]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    TRAP::FileSystem::Shutdown();
}

TEST_CASE("TRAP::FileSystem::ReadFile()", "[filesystem][readfile]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file")
    {
        // TRAP::FileSystem::WriteFile(TestFilesPath / "read.bin", {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'});
        const auto data = TRAP::FileSystem::ReadFile(TestFilesPath / "read.bin");
        REQUIRE(data.Value() == std::vector<u8>{'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("Empty file")
    {
        // TRAP::FileSystem::WriteFile(TestFilesPath / "read_empty.bin", {});
        const auto data = TRAP::FileSystem::ReadFile(TestFilesPath / "read_empty.bin");
        REQUIRE(data.Value().empty());
    }

    SECTION("Empty path")
    {
        const auto data = TRAP::FileSystem::ReadFile("");
        REQUIRE(!data.HasValue());
    }

    SECTION("Folder path")
    {
        const auto data = TRAP::FileSystem::ReadFile(TestFilesPath);
        REQUIRE(!data.HasValue());
    }
}

TEST_CASE("TRAP::FileSystem::ReadTextFile()", "[filesystem][readtextfile]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file")
    {
        // TRAP::FileSystem::WriteTextFile(TestFilesPath / "read.txt", "Hello\nWorld");
        const auto data = TRAP::FileSystem::ReadTextFile(TestFilesPath / "read.txt");
        REQUIRE(data.Value() == "Hello\nWorld");
    }

    SECTION("Empty file")
    {
        // TRAP::FileSystem::WriteTextFile(TestFilesPath / "read_empty.txt", "");
        const auto data = TRAP::FileSystem::ReadTextFile(TestFilesPath / "read_empty.txt");
        REQUIRE(data.Value().empty());
    }

    SECTION("Empty path")
    {
        const auto data = TRAP::FileSystem::ReadTextFile("");
        REQUIRE(!data.HasValue());
    }

    SECTION("Folder path")
    {
        const auto data = TRAP::FileSystem::ReadTextFile(TestFilesPath);
        REQUIRE(!data.HasValue());
    }
}

TEST_CASE("TRAP::FileSystem::WriteFile()", "[filesystem][writefile]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file")
    {
        const std::vector<u8> data = std::vector<u8>{'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "write.bin", data, TRAP::FileSystem::WriteMode::Overwrite));
        REQUIRE(TRAP::FileSystem::ReadFile(TestFilesPath / "write.bin").Value() == data);

        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "write.bin", data, TRAP::FileSystem::WriteMode::Append));
        std::vector<u8> data2(data);
        data2.insert(data2.end(), data.begin(), data.end());
        REQUIRE(TRAP::FileSystem::ReadFile(TestFilesPath / "write.bin").Value() == data2);
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "write.bin"));
    }

    SECTION("Empty file")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "write_empty.bin", {}, TRAP::FileSystem::WriteMode::Overwrite));
        REQUIRE(TRAP::FileSystem::ReadFile(TestFilesPath / "write_empty.bin").Value().empty());
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "write_empty.bin"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::WriteFile("", {}));
    }

    SECTION("Folder path")
    {
        REQUIRE(!TRAP::FileSystem::WriteFile(TestFilesPath, {}));
    }
}

TEST_CASE("TRAP::FileSystem::WriteTextFile()", "[filesystem][writetextfile]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file")
    {
        static constexpr std::string_view data = "Hello\nWorld!\n";

        REQUIRE(TRAP::FileSystem::WriteTextFile(TestFilesPath / "write.txt", data, TRAP::FileSystem::WriteMode::Overwrite));
        REQUIRE(TRAP::FileSystem::ReadTextFile(TestFilesPath / "write.txt").Value() == data);

        REQUIRE(TRAP::FileSystem::WriteTextFile(TestFilesPath / "write.txt", data, TRAP::FileSystem::WriteMode::Append));
        std::string data2(data);
        data2.insert(data2.end(), data.begin(), data.end());
        REQUIRE(TRAP::FileSystem::ReadTextFile(TestFilesPath / "write.txt").Value() == data2);
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "write.txt"));
    }

    SECTION("Empty file")
    {
        REQUIRE(TRAP::FileSystem::WriteTextFile(TestFilesPath / "write_empty.txt", "", TRAP::FileSystem::WriteMode::Overwrite));
        REQUIRE(TRAP::FileSystem::ReadTextFile(TestFilesPath / "write_empty.txt").Value().empty());
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "write_empty.txt"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::WriteTextFile("", ""));
    }

    SECTION("Folder path")
    {
        REQUIRE(!TRAP::FileSystem::WriteTextFile(TestFilesPath, ""));
    }
}

TEST_CASE("TRAP::FileSystem::CreateFolder()", "[filesystem][createfolder]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "create_folder"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "create_folder"));
    }

    SECTION("Existing path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath));
    }

    SECTION("File path")
    {
        REQUIRE(!TRAP::FileSystem::CreateFolder(TestFilesPath / "read.txt"));
    }

#ifdef TRAP_PLATFORM_WINDOWS
    SECTION("Invalid path")
    {
        REQUIRE(!TRAP::FileSystem::CreateFolder(TestFilesPath / "create:!folder"));
    }
#endif /*TRAP_PLATFORM_WINDOWS*/

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::CreateFolder(""));
    }
}

TEST_CASE("TRAP::FileSystem::Delete()", "[filesystem][delete]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "delete_file", {}));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "delete_file"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "delete_folder"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "delete_folder"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::Delete(""));
    }
}

TEST_CASE("TRAP::FileSystem::Move()", "[filesystem][move]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "move_file", {}));
        REQUIRE(TRAP::FileSystem::Move(TestFilesPath / "move_file", TestFilesPath / "move_file_dst"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "move_file_dst"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "move_folder"));
        REQUIRE(TRAP::FileSystem::Move(TestFilesPath / "move_folder", TestFilesPath / "move_folder_dst"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "move_folder_dst"));
    }

    SECTION("Empty source path")
    {
        REQUIRE(!TRAP::FileSystem::Move("", TestFilesPath / "move_folder_empty_dst"));
    }

    SECTION("Empty destination path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "move_folder_empty"));
        REQUIRE(!TRAP::FileSystem::Move(TestFilesPath / "move_folder_empty", ""));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "move_folder_empty"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::Move("", ""));
    }
}

TEST_CASE("TRAP::FileSystem::Copy()", "[filesystem][copy]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "copy_file", {}));
        REQUIRE(TRAP::FileSystem::Copy(TestFilesPath / "copy_file", TestFilesPath / "copy_file_dst", false));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_file"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_file_dst"));
    }

    SECTION("Valid file path (overwrite existing)")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "copy_file_overwrite", {}));
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "copy_file_overwrite_dst", {}));
        REQUIRE(TRAP::FileSystem::Copy(TestFilesPath / "copy_file_overwrite", TestFilesPath / "copy_file_overwrite_dst", true));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_file_overwrite"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_file_overwrite_dst"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "copy_folder"));
        REQUIRE(TRAP::FileSystem::Copy(TestFilesPath / "copy_folder", TestFilesPath / "copy_folder_dst", false));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_folder"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_folder_dst"));
    }

    SECTION("Valid folder path (overwrite existing)")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "copy_folder_overwrite"));
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "copy_folder_overwrite_dst"));
        REQUIRE(TRAP::FileSystem::Copy(TestFilesPath / "copy_folder_overwrite", TestFilesPath / "copy_folder_overwrite_dst", true));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_folder_overwrite"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_folder_overwrite_dst"));
    }

    SECTION("Empty source path")
    {
        REQUIRE(!TRAP::FileSystem::Copy("", TestFilesPath / "copy_empty_src", false));
        REQUIRE(!TRAP::FileSystem::Copy("", TestFilesPath / "copy_empty_src", true));
    }

    SECTION("Empty destination path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "copy_folder_empty"));
        REQUIRE(!TRAP::FileSystem::Copy(TestFilesPath / "copy_folder_empty", "", false));
        REQUIRE(!TRAP::FileSystem::Copy(TestFilesPath / "copy_folder_empty", "", true));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "copy_folder_empty"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::Copy("", "", false));
        REQUIRE(!TRAP::FileSystem::Copy("", "", true));
    }
}

TEST_CASE("TRAP::FileSystem::Rename()", "[filesystem][rename]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "rename_file", {}));
        REQUIRE(TRAP::FileSystem::Rename(TestFilesPath / "rename_file", "rename_file_dst"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "rename_file_dst"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "rename_folder"));
        REQUIRE(TRAP::FileSystem::Rename(TestFilesPath / "rename_folder", "rename_folder_dst"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "rename_folder_dst"));
    }

    SECTION("Empty source path")
    {
        REQUIRE(!TRAP::FileSystem::Rename("", "rename_empty_dst"));
    }

    SECTION("Empty destination name")
    {
        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "rename_folder_empty_dst"));
        REQUIRE(!TRAP::FileSystem::Rename(TestFilesPath / "rename_folder_empty_dst", ""));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "rename_folder_empty_dst"));

        REQUIRE(TRAP::FileSystem::WriteFile(TestFilesPath / "rename_file_empty_dst", {}));
        REQUIRE(!TRAP::FileSystem::Rename(TestFilesPath / "rename_file_empty_dst", ""));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "rename_file_empty_dst"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::Rename("", ""));
    }
}

TEST_CASE("TRAP::FileSystem::Exists()", "[filesystem][exists]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::Exists(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::Exists(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::Exists(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetSize()", "[filesystem][getsize]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        const auto size = TRAP::FileSystem::GetSize(TestFilesPath / "read.txt", false);
        REQUIRE(size);
        // REQUIRE(*size == 11);

        const auto size2 = TRAP::FileSystem::GetSize(TestFilesPath / "read.txt", true);
        REQUIRE(size2);
        // REQUIRE(*size2 == 11);
    }

    SECTION("Valid folder path")
    {
        const auto size = TRAP::FileSystem::GetSize(TestFilesPath, false);
        REQUIRE(size);

        const auto size2 = TRAP::FileSystem::GetSize(TestFilesPath, true);
        REQUIRE(size2);
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetSize("", false));
        REQUIRE(!TRAP::FileSystem::GetSize("", true));
    }
}

TEST_CASE("TRAP::FileSystem::GetLastWriteTime()", "[filesystem][getlastwritetime]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::GetLastWriteTime(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::GetLastWriteTime(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetLastWriteTime(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetFileNameWithEnding()", "[filesystem][getfilenamewithending]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        const auto fileName = TRAP::FileSystem::GetFileNameWithEnding(TestFilesPath / "read.txt");
        REQUIRE(fileName);
        REQUIRE(*fileName == "read.txt");
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileNameWithEnding(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileNameWithEnding(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetFileNameWithoutEnding()", "[filesystem][getfilenamewithoutending]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        const auto fileName = TRAP::FileSystem::GetFileNameWithoutEnding(TestFilesPath / "read.txt");
        REQUIRE(fileName);
        REQUIRE(*fileName == "read");
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileNameWithoutEnding(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileNameWithoutEnding(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetFileEnding()", "[filesystem][getfileending]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        const auto fileName = TRAP::FileSystem::GetFileEnding(TestFilesPath / "read.txt");
        REQUIRE(fileName);
        REQUIRE(*fileName == ".txt");

        const auto fileName2 = TRAP::FileSystem::GetFileEnding(TestFilesPath / "read");
        REQUIRE(!fileName2);
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileEnding(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileEnding(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetFolderPath()", "[filesystem][getfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        const auto folderPath = TRAP::FileSystem::GetFolderPath(TestFilesPath / "read.txt");
        REQUIRE(folderPath);
        REQUIRE(*folderPath == "Testfiles/FileSystem");
    }

    SECTION("Valid folder path")
    {
        const auto folderPath = TRAP::FileSystem::GetFolderPath(TestFilesPath);
        REQUIRE(folderPath);
        REQUIRE(*folderPath == "Testfiles/FileSystem");
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::GetFileNameWithoutEnding(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetEngineTempFolderPath()", "[filesystem][getenginetempfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    const auto engineTempPath = TRAP::FileSystem::GetEngineTempFolderPath();
    REQUIRE(engineTempPath);
    REQUIRE(!engineTempPath->empty());
}

TEST_CASE("TRAP::FileSystem::GetGameTempFolderPath()", "[filesystem][getgametempfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    TRAP::Application app{"UnitTest"};

    const auto gameTempPath = TRAP::FileSystem::GetGameTempFolderPath();
    REQUIRE(gameTempPath);
    REQUIRE(!gameTempPath->empty());
}

TEST_CASE("TRAP::FileSystem::GetDocumentsFolderPath()", "[filesystem][getdocumentsfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    const auto docsPath = TRAP::FileSystem::GetDocumentsFolderPath();
    REQUIRE(docsPath);
    REQUIRE(!docsPath->empty());
}

TEST_CASE("TRAP::FileSystem::GetGameDocumentsFolderPath()", "[filesystem][getgamedocumentsfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    TRAP::Application app{"UnitTest"};

    const auto gameDocsPath = TRAP::FileSystem::GetGameDocumentsFolderPath();
    REQUIRE(gameDocsPath);
    REQUIRE(!gameDocsPath->empty());
}

TEST_CASE("TRAP::FileSystem::GetGameLogFolderPath()", "[filesystem][getgamelogfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);
    TRAP::Application app{"UnitTest"};

    const auto gameLogPath = TRAP::FileSystem::GetGameLogFolderPath();
    REQUIRE(gameLogPath);
    REQUIRE(!gameLogPath->empty());
}

TEST_CASE("TRAP::FileSystem::IsEquivalent()", "[filesystem][isequivalent]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::IsEquivalent(TestFilesPath / "read.txt", TestFilesPath / "read.txt"));
        REQUIRE(!TRAP::FileSystem::IsEquivalent(TestFilesPath / "read.txt", TestFilesPath / "read.bin"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::IsEquivalent(TestFilesPath, TestFilesPath));
        REQUIRE(!TRAP::FileSystem::IsEquivalent(TestFilesPath, ".." / TestFilesPath));
    }

    SECTION("Mixed file and folder path")
    {
        REQUIRE(!TRAP::FileSystem::IsEquivalent(TestFilesPath / "read.bin", TestFilesPath));
        REQUIRE(!TRAP::FileSystem::IsEquivalent(TestFilesPath, TestFilesPath / "read.txt"));
    }

    SECTION("Empty source path")
    {
        REQUIRE(!TRAP::FileSystem::IsEquivalent("", TestFilesPath / "read.txt"));
    }

    SECTION("Empty destination path")
    {
        REQUIRE(!TRAP::FileSystem::IsEquivalent(TestFilesPath / "read.txt", ""));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::IsEquivalent("", ""));
    }
}

TEST_CASE("TRAP::FileSystem::IsFolder()", "[filesystem][isfolder]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(!TRAP::FileSystem::IsFolder(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::IsFolder(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::IsFolder(""));
    }
}

TEST_CASE("TRAP::FileSystem::IsFile()", "[filesystem][isfile]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::IsFile(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::IsFile(TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::IsFile(""));
    }
}

TEST_CASE("TRAP::FileSystem::IsEmpty()", "[filesystem][isempty]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(!TRAP::FileSystem::IsEmpty(TestFilesPath / "read.txt"));
        REQUIRE(TRAP::FileSystem::IsEmpty(TestFilesPath / "read_empty.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::IsEmpty(TestFilesPath));

        REQUIRE(TRAP::FileSystem::CreateFolder(TestFilesPath / "folder_empty"));
        REQUIRE(TRAP::FileSystem::IsEmpty(TestFilesPath / "folder_empty"));
        REQUIRE(TRAP::FileSystem::Delete(TestFilesPath / "folder_empty"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::IsEmpty(""));
    }
}

TEST_CASE("TRAP::FileSystem::IsAbsolute()", "[filesystem][isabsolute]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(!TRAP::FileSystem::IsAbsolute(TestFilesPath / "read.txt"));

        const auto currWorkFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
        REQUIRE(currWorkFolder);
        REQUIRE(TRAP::FileSystem::IsAbsolute(*currWorkFolder / TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(!TRAP::FileSystem::IsAbsolute(TestFilesPath));

        const auto currWorkFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
        REQUIRE(currWorkFolder);
        REQUIRE(TRAP::FileSystem::IsAbsolute(*currWorkFolder / TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::IsAbsolute(""));
    }
}

TEST_CASE("TRAP::FileSystem::IsRelative()", "[filesystem][isrelative]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::IsRelative(TestFilesPath / "read.txt"));

        const auto currWorkFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
        REQUIRE(currWorkFolder);
        REQUIRE(!TRAP::FileSystem::IsRelative(*currWorkFolder / TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::IsRelative(TestFilesPath));

        const auto currWorkFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
        REQUIRE(currWorkFolder);
        REQUIRE(!TRAP::FileSystem::IsRelative(*currWorkFolder / TestFilesPath));
    }

    SECTION("Empty path")
    {
        REQUIRE(TRAP::FileSystem::IsRelative(""));
    }
}

TEST_CASE("TRAP::FileSystem::ToAbsolutePath()", "[filesystem][toabsolutepath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::ToAbsolutePath(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        const auto absPath = TRAP::FileSystem::ToAbsolutePath(TestFilesPath);
        REQUIRE(absPath);
        REQUIRE(TRAP::FileSystem::IsEquivalent(*absPath, "Testfiles/FileSystem"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::ToAbsolutePath(""));
    }
}

TEST_CASE("TRAP::FileSystem::ToCanonicalAbsolutePath()", "[filesystem][tocanonicalabsolutepath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    SECTION("Valid file path")
    {
        REQUIRE(TRAP::FileSystem::ToCanonicalAbsolutePath(TestFilesPath / "./read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::ToCanonicalAbsolutePath(TestFilesPath));
        const auto absPath = TRAP::FileSystem::ToCanonicalAbsolutePath(TestFilesPath / "././");
        REQUIRE(TRAP::FileSystem::IsEquivalent(*absPath, "Testfiles/FileSystem"));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::ToCanonicalAbsolutePath(""));
    }
}

TEST_CASE("TRAP::FileSystem::ToRelativePath()", "[filesystem][torelativepath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    const auto cwd = TRAP::FileSystem::GetCurrentWorkingFolderPath();
    REQUIRE(cwd);

    SECTION("Valid file path")
    {
        const auto relPath = TRAP::FileSystem::ToRelativePath(*cwd / TestFilesPath / "read.txt");
        REQUIRE(relPath);
        REQUIRE(*relPath == TestFilesPath / "read.txt");

        const auto relPath2 = TRAP::FileSystem::ToRelativePath(*cwd / TestFilesPath / "read.txt", *cwd);
        REQUIRE(relPath2);
        REQUIRE(*relPath2 == TestFilesPath / "read.txt");
    }

    SECTION("Valid folder path")
    {
        const auto relPath = TRAP::FileSystem::ToRelativePath(*cwd / TestFilesPath);
        REQUIRE(relPath);
        REQUIRE(*relPath == "Testfiles/FileSystem");

        const auto relPath2 = TRAP::FileSystem::ToRelativePath(*cwd / TestFilesPath, *cwd);
        REQUIRE(relPath2);
        REQUIRE(*relPath2 == "Testfiles/FileSystem");
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::ToRelativePath(""));
        REQUIRE(!TRAP::FileSystem::ToRelativePath("", *cwd));
    }
}

TEST_CASE("TRAP::FileSystem::SetCurrentWorkingFolderPath()", "[filesystem][setcurrentworkingfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    const auto cwd = TRAP::FileSystem::GetCurrentWorkingFolderPath();
    REQUIRE(cwd);

    SECTION("Valid file path")
    {
        REQUIRE(!TRAP::FileSystem::SetCurrentWorkingFolderPath(TestFilesPath / "read.txt"));
    }

    SECTION("Valid folder path")
    {
        REQUIRE(TRAP::FileSystem::SetCurrentWorkingFolderPath(TestFilesPath));
        REQUIRE(TRAP::FileSystem::SetCurrentWorkingFolderPath(*cwd));
    }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::SetCurrentWorkingFolderPath(""));
    }
}

TEST_CASE("TRAP::FileSystem::GetCurrentWorkingFolderPath()", "[filesystem][getcurrentworkingfolderpath]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    const auto cwd = TRAP::FileSystem::GetCurrentWorkingFolderPath();
    REQUIRE(cwd);
    REQUIRE(!cwd->empty());
}

TEST_CASE("TRAP::FileSystem::OpenInFileBrowser()", "[filesystem][openinfilebrowser]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    //Note: Requires changes to TRAP::FileSystem
    // SECTION("Valid file path")
    // {
    //     REQUIRE(!TRAP::FileSystem::OpenInFileBrowser(TestFilesPath / "read.txt"));
    // }

    //Note: Requires changes to TRAP::FileSystem
    // SECTION("Valid folder path")
    // {
    //     REQUIRE(TRAP::FileSystem::OpenInFileBrowser(TestFilesPath));
    // }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::OpenInFileBrowser(""));
    }
}

TEST_CASE("TRAP::FileSystem::OpenExternally()", "[filesystem][openexternally]")
{
    TRAP::TRAPLog.SetImportance(TRAP::Log::Level::Critical);

    //Note: Requires changes to TRAP::FileSystem
    // SECTION("Valid file path")
    // {
    //     REQUIRE(!TRAP::FileSystem::OpenExternally(TestFilesPath / "read.txt"));
    // }

    //Note: Requires changes to TRAP::FileSystem
    // SECTION("Valid folder path")
    // {
    //     REQUIRE(TRAP::FileSystem::OpenExternally(TestFilesPath));
    // }

    SECTION("Empty path")
    {
        REQUIRE(!TRAP::FileSystem::OpenExternally(""));
    }
}
