#include "FileSystemTests.h"
#include "FileSystem/FileSystem.h"

#include <filesystem>
#include <fstream>

void RunValidTests();
void RunInvalidTests();

//-------------------------------------------------------------------------------------------------------------------//

FileSystemTests::FileSystemTests()
    : Layer("HeadlessFileSystemTests")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void FileSystemTests::OnAttach()
{
    TP_INFO("Running FileSystemTests...");
    RunValidTests();
    RunInvalidTests();

	TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void RunValidTests()
{
    TP_INFO("Running valid tests...");

    //ReadFile()
    TP_INFO("ReadFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::ReadFile("Assets/Textures/vulkanlogo.png");
        TRAP_ASSERT(res, "ReadFile() failed!");

        //Check
        std::ifstream file("Assets/Textures/vulkanlogo.png", std::ios::binary | std::ios::ate);
        TRAP_ASSERT(file.is_open() && file.good(), "ReadFile() failed! (ifstream failed)");
        const std::size_t fileSize = static_cast<std::size_t>(file.tellg());
        std::vector<uint8_t> validRes(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(validRes.data()), fileSize);
        file.close();

        TRAP_ASSERT(*res == validRes, "ReadFile() failed!");
    }

    //ReadTextFile()
    TP_INFO("ReadTextFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::ReadTextFile("Assets/Shaders/emboss.compute.shader");
        TRAP_ASSERT(res, "ReadTextFile() failed!");

        //Check
        std::ifstream file("Assets/Shaders/emboss.compute.shader", std::ios::ate);
        TRAP_ASSERT(file.is_open() && file.good(), "ReadTextFile() failed! (ifstream failed)");
        const std::size_t fileSize = static_cast<std::size_t>(file.tellg());
        std::string validRes(fileSize, '\0');
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(validRes.data()), fileSize);

        TRAP_ASSERT(*res == validRes, "ReadTextFile() failed!");
    }

    //WriteFile() (overwrite mode)
    TP_INFO("WriteFile (overwrite mode)...");
    {
        //Setup
        std::vector<uint8_t> data{};
        for(uint32_t i = 0; i < 100; ++i)
            data.insert(data.end(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

        //Test
        const auto res = TRAP::FileSystem::WriteFile("test.bin", data, TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "WriteFile() failed!");

        //Check
        const auto readContents = TRAP::FileSystem::ReadFile("test.bin");
        TRAP_ASSERT(readContents, "WriteFile() failed!");
        TRAP_ASSERT(*readContents == data, "WriteFile() failed!");

        //Cleanup
        std::filesystem::remove_all("test.bin");
    }

    //WriteFile() (append mode)
    TP_INFO("WriteFile (append mode)...");
    {
        //Setup
        std::vector<uint8_t> data{};
        for(uint32_t i = 0; i < 100; ++i)
            data.insert(data.end(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        const auto res = TRAP::FileSystem::WriteFile("test.bin", data, TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "WriteFile() failed!");

        //Test
        data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        const auto res1 = TRAP::FileSystem::WriteFile("test.bin", data, TRAP::FileSystem::WriteMode::Append);
        TRAP_ASSERT(res1, "WriteFile() failed!");

        //Check
        for(uint32_t i = 0; i < 100; ++i)
            data.insert(data.end(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        const auto readContents = TRAP::FileSystem::ReadFile("test.bin");
        TRAP_ASSERT(readContents, "WriteFile() failed!");
        TRAP_ASSERT(*readContents == data, "WriteFile() failed!");

        //Cleanup
        std::filesystem::remove_all("test.bin");
    }

    //WriteTextFile() (overwrite mode)
    TP_INFO("WriteTextFile (overwrite mode)...");
    {
        //Setup
        std::string data{};
        for(uint32_t i = 0; i < 100; ++i)
            data += "Some long string to test the filesystem api. ";

        //Test
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", data, TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "WriteTextFile() failed!");

        //Check
        const auto readContents = TRAP::FileSystem::ReadTextFile("test.txt");
        TRAP_ASSERT(readContents, "WriteTextFile() failed!");
        TRAP_ASSERT(*readContents == data, "WriteTextFile() failed!");

        //Cleanup
        std::filesystem::remove_all("test.txt");
    }

    //WriteTextFile() (append mode)
    TP_INFO("WriteTextFile (append mode)...");
    {
        //Setup
        std::string data{};
        for(uint32_t i = 0; i < 100; ++i)
            data += "Some long string to test the filesystem api. ";
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", data, TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "WriteTextFile() failed!");

        //Test
        data = "Some long string to test the filesystem api. ";
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", data, TRAP::FileSystem::WriteMode::Append);
        TRAP_ASSERT(res1, "WriteTextFile() failed!");

        //Check
        for(uint32_t i = 0; i < 100; ++i)
            data += "Some long string to test the filesystem api. ";
        const auto readContents = TRAP::FileSystem::ReadTextFile("test.txt");
        TRAP_ASSERT(readContents, "WriteTextFile() failed!");
        TRAP_ASSERT(*readContents == data, "WriteTextFile() failed!");

        //Cleanuo
        std::filesystem::remove_all("test.txt");
    }

    //CreateFolder()
    TP_INFO("CreateFolder...");
    {
        //Test
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "CreateFolder() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test"), "CreateFolder() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res1, "CreateFolder() failed!");

        //Cleanup
        std::filesystem::remove_all("test");
    }

    //Delete() (File)
    TP_INFO("Delete (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "Delete() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res1, "Delete() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("test.txt"), "Delete() failed!");
    }

    //Delete() (Folder)
    TP_INFO("Delete (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Delete() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/1.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Delete() failed!");
        const auto res2 = TRAP::FileSystem::WriteTextFile("test/2.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res2, "Delete() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "Delete() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("test"), "Delete() failed!");
    }

    //Move() (File)
    TP_INFO("Move (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "Move() failed!");

        //Test
        const bool res1 = TRAP::FileSystem::Move("test.txt", "test.txt");
        TRAP_ASSERT(res1, "Move() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test.txt"), "Move() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Move("test.txt", "test_moved.txt");
        TRAP_ASSERT(res2, "Move() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("test.txt"), "Move() failed!");
        TRAP_ASSERT(std::filesystem::exists("test_moved.txt"), "Move() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test_moved.txt");
        TRAP_ASSERT(res3, "Move() failed!");
    }

    //Move() (Folder)
    TP_INFO("Move (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Move() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/1.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Move() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Move("test", "test");
        TRAP_ASSERT(res2, "Move() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test"), "Move() failed!");

        //Test
        const bool res3 = TRAP::FileSystem::Move("test", "test_moved");
        TRAP_ASSERT(res3, "Move() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("test.txt"), "Move() failed!");
        TRAP_ASSERT(std::filesystem::exists("test_moved"), "Move() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test_moved");
        TRAP_ASSERT(res4, "Move() failed!");
    }

    //Copy() (File)
    TP_INFO("Copy (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "Copy() failed!");

        //Test
        const bool res1 = TRAP::FileSystem::Copy("test.txt", "test_copied.txt");
        TRAP_ASSERT(res1, "Copy() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test_copied.txt"), "Copy() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Copy("test.txt", "test_copied.txt", true);
        TRAP_ASSERT(res2, "Copy() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test_copied.txt"), "Copy() failed!");

        //Setup
        const auto res3 = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res3, "Copy() failed!");

        //Test
        const bool res4 = TRAP::FileSystem::Copy("test.txt", "test/", true);
        TRAP_ASSERT(res4, "Copy() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test/test.txt"), "Copy() failed!");

        //Cleanup
        const auto res5 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res5, "Copy() failed!");
        const auto res6 = TRAP::FileSystem::Delete("test_copied.txt");
        TRAP_ASSERT(res6, "Copy() failed!");
        const auto res7 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res7, "Copy() failed!");
    }

    //Copy() (Folder)
    TP_INFO("Copy (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Copy() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/1.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Copy() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Copy("test", "test_copied");
        TRAP_ASSERT(res2, "Copy() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test_copied"), "Copy() failed!");

        //Test
        const bool res3 = TRAP::FileSystem::Copy("test", "test_copied", true);
        TRAP_ASSERT(res3, "Copy() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test_copied"), "Copy() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res4, "Copy() failed!");
        const auto res5 = TRAP::FileSystem::Delete("test_copied");
        TRAP_ASSERT(res5, "Copy() failed!");
    }

    //Rename (File)
    TP_INFO("Rename (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Rename() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Rename() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Rename("test/test.txt", "test_renamed.txt");
        TRAP_ASSERT(res2, "Rename() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test/test_renamed.txt"), "Rename() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "Rename() failed!");
    }

    //Rename (Folder)
    TP_INFO("Rename (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Rename() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Rename() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Rename("test", "test_renamed");
        TRAP_ASSERT(res2, "Rename() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test_renamed"), "Rename() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test_renamed");
        TRAP_ASSERT(res3, "Rename() failed!");
    }

    //Exists (File)
    TP_INFO("Exists (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "Exists() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::Exists("test.txt");
        TRAP_ASSERT(res1, "Exists() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test.txt"), "Exists() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Exists("Some/Random/File.txt");
        TRAP_ASSERT(!res2, "Exists() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res3, "Exists() failed!");
    }

    //Exists (Folder)
    TP_INFO("Exists (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Exists() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::Exists("test");
        TRAP_ASSERT(res1, "Exists() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test"), "Exists() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Exists("Some/Random/File");
        TRAP_ASSERT(!res2, "Exists() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "Exists() failed!");
    }

    //GetSize (File)
    TP_INFO("GetSize (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "GetSize() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::GetSize("test.txt");
        TRAP_ASSERT(res1, "GetSize() failed!");

        //Check
        std::ifstream file("test.txt", std::ios::binary | std::ios::ate);
        if(!file.good() || !file.is_open())
            TRAP_ASSERT(false, "GetSize() failed!");
        const auto size = file.tellg();
        file.close();
        TRAP_ASSERT(*res1 == static_cast<uintmax_t>(size), "GetSize() failed!");

        //Cleanup
        const auto res2 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res2, "GetSize() failed!");
    }

    //GetSize (Folder)
    TP_INFO("GetSize (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "GetSize() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "GetSize() failed!");
        const auto res2 = TRAP::FileSystem::CreateFolder("test/test");
        TRAP_ASSERT(res2, "GetSize() failed!");
        const auto res3 = TRAP::FileSystem::WriteTextFile("test/test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res3, "GetSize() failed!");

        //Test
        const auto res4 = TRAP::FileSystem::GetSize("test", false);
        TRAP_ASSERT(res4, "GetSize() failed!");

        //Check
        uintmax_t fileSize = 0;
        for(const auto& entry : std::filesystem::directory_iterator("test"))
        {
            if(entry.is_regular_file())
            {
                std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
                if(!file.good() || !file.is_open())
                    TRAP_ASSERT(false, "GetSize() failed!");
                const auto size = file.tellg();
                file.close();

                fileSize += size;
            }
        }
        TRAP_ASSERT(*res4 == fileSize, "GetSize() failed!");

        //Test
        const auto res5 = TRAP::FileSystem::GetSize("test", true);
        TRAP_ASSERT(res5, "GetSize() failed!");

        //Check
        fileSize = 0;
        for(const auto& entry : std::filesystem::recursive_directory_iterator("test"))
        {
            if(entry.is_regular_file())
            {
                std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
                if(!file.good() || !file.is_open())
                    TRAP_ASSERT(false, "GetSize() failed!");
                const auto size = file.tellg();
                file.close();

                fileSize += size;
            }
        }
        TRAP_ASSERT(*res5 == fileSize, "GetSize() failed!");

        //Cleanup
        const auto res6 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res6, "GetSize() failed!");
    }

    //GetLastWriteTime (File)
    TP_INFO("GetLastWriteTime (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "GetLastWriteTime() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::GetLastWriteTime("test.txt");
        TRAP_ASSERT(res1, "GetLastWriteTime() failed!");

        //Cleanup
        const auto res2 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res2, "GetLastWriteTime() failed!");
    }

    //GetLastWriteTime (Folder)
    TP_INFO("GetLastWriteTime (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "GetLastWriteTime() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "GetLastWriteTime() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::GetLastWriteTime("test");
        TRAP_ASSERT(res2, "GetLastWriteTime() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "GetLastWriteTime() failed!");
    }

    //GetFileNameWithEnding
    TP_INFO("GetFileNameWithEnding...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetFileNameWithEnding("test/test.txt");
        TRAP_ASSERT(res, "GetFileNameWithEnding() failed!");

        //Check
        TRAP_ASSERT(*res == "test.txt", "GetLastWriteTime() failed!");
    }

    //GetFileNameWithoutEnding
    TP_INFO("GetFileNameWithoutEnding...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetFileNameWithoutEnding("test/test.txt");
        TRAP_ASSERT(res, "GetFileNameWithoutEnding() failed!");

        //Check
        TRAP_ASSERT(*res == "test", "GetFileNameWithoutEnding() failed!");
    }

    //GetFileEnding
    TP_INFO("GetFileEnding...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetFileEnding("test/test.txt");
        TRAP_ASSERT(res, "GetFileEnding() failed!");

        //Check
        TRAP_ASSERT(*res == ".txt", "GetFileEnding() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::GetFileEnding("test/test");
        TRAP_ASSERT(!res1, "GetFileEnding() failed!");
    }

    //GetFolderPath
    TP_INFO("GetFolderPath...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetFolderPath("test/test.txt");
        TRAP_ASSERT(res, "GetFolderPath() failed!");

        //Check
        TRAP_ASSERT(res == "test", "GetFolderPath() failed!");
    }

    //IsEquivalent()
    TP_INFO("IsEquivalent...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "IsEquivalent() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "IsEquivalent() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::IsEquivalent("test.txt", "test.txt");
        TRAP_ASSERT(res2, "IsEquivalent() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::IsEquivalent("test", "test");
        TRAP_ASSERT(res3, "IsEquivalent() failed!");

        //Test
        const auto res4 = TRAP::FileSystem::IsEquivalent("test", "test.txt");
        TRAP_ASSERT(!res4, "IsEquivalent() failed!");

        //Cleanup
        const auto res5 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res5, "IsEquivalent() failed!");
        const auto res6 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res6, "IsEquivalent() failed!");
    }

    //IsFolder()
    TP_INFO("IsFolder...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "IsFolder() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "IsFolder() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::IsFolder("test.txt");
        TRAP_ASSERT(!res2, "IsFolder() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::IsFolder("test");
        TRAP_ASSERT(res3, "IsFolder() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res4, "IsFolder() failed!");
        const auto res5 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res5, "IsFolder() failed!");
    }

    //IsFile()
    TP_INFO("IsFile...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "IsFile() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "IsFile() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::IsFile("test.txt");
        TRAP_ASSERT(res2, "IsFile() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::IsFile("test");
        TRAP_ASSERT(!res3, "IsFile() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res4, "IsFile() failed!");
        const auto res5 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res5, "IsFile() failed!");
    }

    //IsEmpty()
    TP_INFO("IsEmpty...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "IsEmpty() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", "", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "IsEmpty() failed!");
        const auto res2 = TRAP::FileSystem::WriteTextFile("test1.txt", "Some text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res2, "IsEmpty() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::IsEmpty("test.txt");
        TRAP_ASSERT(res3, "IsEmpty() failed!");

        //Test
        const auto res4 = TRAP::FileSystem::IsEmpty("test1.txt");
        TRAP_ASSERT(!res4, "IsEmpty() failed!");

        //Test
        const auto res5 = TRAP::FileSystem::IsEmpty("test");
        TRAP_ASSERT(res5, "IsEmpty() failed!");

        //Setup
        const auto res6 = TRAP::FileSystem::Move("test.txt", "test/test.txt");
        TRAP_ASSERT(res6, "IsEmpty() failed!");

        //Test
        const auto res7 = TRAP::FileSystem::IsEmpty("test");
        TRAP_ASSERT(!res7, "IsEmpty() failed!");

        //Cleanup
        const auto res8 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res8, "IsEmpty() failed!");
        const auto res9 = TRAP::FileSystem::Delete("test1.txt");
        TRAP_ASSERT(res9, "IsEmpty() failed!");
    }

    //IsAbsolute()
    TP_INFO("IsAbsolute...");
    {
        //Test
        const auto res = TRAP::FileSystem::IsAbsolute("test.txt");
        TRAP_ASSERT(!res, "ToAbsolutePath() failed!");

        //Test
#ifdef TRAP_PLATFORM_WINDOWS
        const auto res1 = TRAP::FileSystem::IsAbsolute("C:\\test.txt");
#else
        const auto res1 = TRAP::FileSystem::IsAbsolute("/test.txt");
#endif
        TRAP_ASSERT(res1, "ToAbsolutePath() failed!");
    }

    //IsRelative()
    TP_INFO("IsRelative...");
    {
        //Test
        const auto res = TRAP::FileSystem::IsRelative("test.txt");
        TRAP_ASSERT(res, "IsRelative() failed!");

        //Test
#ifdef TRAP_PLATFORM_WINDOWS
        const auto res1 = TRAP::FileSystem::IsRelative("C:\\test.txt");
#else
        const auto res1 = TRAP::FileSystem::IsRelative("/test.txt");
#endif
        TRAP_ASSERT(!res1, "IsRelative() failed!");
    }

    //ToAbsolutePath()
    TP_INFO("ToAbsolutePath...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "ToAbsolutePath() failed!");

        //Test
        const auto res1 = TRAP::FileSystem::ToAbsolutePath("test.txt");
        TRAP_ASSERT(res1, "ToAbsolutePath() failed!");

        const auto res2 = TRAP::FileSystem::ToAbsolutePath(*res1);
        TRAP_ASSERT(res2, "ToAbsolutePath() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res3, "ToAbsolutePath() failed!");
    }

    //ToRelativePath()
    TP_INFO("ToRelativePath...");
    {
        //Setup
        const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res, "ToRelativePath() failed!");
        const auto res1 = TRAP::FileSystem::ToAbsolutePath("test.txt");
        TRAP_ASSERT(res1, "ToRelativePath() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::ToRelativePath(*res1);
        TRAP_ASSERT(res2, "ToRelativePath() failed!");

        //Test
        const auto res3 = TRAP::FileSystem::ToRelativePath(*res2);
        TRAP_ASSERT(res3, "ToRelativePath() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res4, "ToRelativePath() failed!");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void RunInvalidTests()
{
    TP_INFO("Running invalid tests...");

    //ReadFile()
    TP_INFO("ReadFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::ReadFile("Some/Invalid/Path");
        TRAP_ASSERT(!res, "ReadFile() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "ReadFile() failed!");
    }

    //ReadTextFile()
    TP_INFO("ReadTextFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::ReadTextFile("Some/Invalid/Path");
        TRAP_ASSERT(!res, "ReadTextFile() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "ReadTextFile() failed!");
    }

    //WriteFile()
    TP_INFO("WriteFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::WriteFile("Some/Invalid/Path", {1, 2, 3}, TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(!res, "WriteFile() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "WriteFile() failed!");
    }

    //WriteTextFile()
    TP_INFO("WriteTextFile...");
    {
        //Test
        const auto res = TRAP::FileSystem::WriteTextFile("Some/Invalid/Path", "LMAO", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(!res, "WriteTextFile() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "WriteTextFile() failed!");
    }

    //CreateFolder()
    TP_INFO("CreateFolder...");
    {
#ifdef TRAP_PLATFORM_WINDOWS
        //Test
        //Folder with characters that are not allowed on Windows/NTFS
        const std::string path = "\\/:*?\"<>|";
        const auto res = TRAP::FileSystem::CreateFolder(path);
        TRAP_ASSERT(!res, "CreateFolder() failed!");
#endif /*TRAP_PLATFORM_WINDOWS*/
    }

    //Delete()
    TP_INFO("Delete...");
    {
        //Test
        const auto res = TRAP::FileSystem::Delete("Some/Invalid/Path");
        TRAP_ASSERT(!res, "Delete() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "Delete() failed!");
    }

    //Move()
    TP_INFO("Move...");
    {
        //Both paths invalid
        {
            //Test
            const auto res = TRAP::FileSystem::Move("Some/Invalid/Path", "Some/Invalid/Path");
            TRAP_ASSERT(!res, "Move() failed!");

            //Check
            TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "Move() failed!");
        }

        //Source path invalid
        {
            //Test
            const auto res1 = TRAP::FileSystem::Move("Some/Invalid/Path", "test");
            TRAP_ASSERT(!res1, "Move() failed!");

            //Check
            TRAP_ASSERT(!std::filesystem::exists("test"), "Move() failed!");
        }

        //Target path invalid
        {
            //Setup
            const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
            TRAP_ASSERT(res, "Move() failed!");

            //Test
            const auto res1 = TRAP::FileSystem::Move("test.txt", "Some/Invalid/Path");
            TRAP_ASSERT(!res1, "Move() failed!");

            //Check
            TRAP_ASSERT(!std::filesystem::exists("Some/Invalid/Path"), "Move() failed!");

            //Cleanup
            const auto res2 = TRAP::FileSystem::Delete("test.txt");
            TRAP_ASSERT(res2, "Move() failed!");
        }

        //Source is file, target is folder
        {
            //Setup
            const auto res = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
            TRAP_ASSERT(res, "Move() failed!");
            const auto res1 = TRAP::FileSystem::CreateFolder("test");
            TRAP_ASSERT(res1, "Move() failed!");

            //Test
            const auto res2 = TRAP::FileSystem::Move("test.txt", "test/");
            TRAP_ASSERT(!res2, "Move() failed!");

            //Check
            TRAP_ASSERT(!std::filesystem::exists("test/test.txt"), "Move() failed!");

            //Cleanup
            const auto res3 = TRAP::FileSystem::Delete("test.txt");
            TRAP_ASSERT(res3, "Move() failed!");
            const auto res4 = TRAP::FileSystem::Delete("test");
            TRAP_ASSERT(res4, "Move() failed!");
        }
    }

    //Copy() (File)
    TP_INFO("Copy (file)...");
    {
        //Setup
        const auto res1 = TRAP::FileSystem::WriteTextFile("test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Copy() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Copy("test.txt", "test.txt");
        TRAP_ASSERT(!res2, "Copy() failed!");

        //Test
        const bool res3 = TRAP::FileSystem::Copy("test.txt", "test.txt", true);
        TRAP_ASSERT(!res3, "Copy() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test.txt");
        TRAP_ASSERT(res4, "Copy() failed!");
    }

    //Copy() (Folder)
    TP_INFO("Copy (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Copy() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/1.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Copy() failed!");

        //Test
        const bool res2 = TRAP::FileSystem::Copy("test", "test");
        TRAP_ASSERT(!res2, "Copy() failed!");

        //Test
        const bool res3 = TRAP::FileSystem::Copy("test", "test", true);
        TRAP_ASSERT(!res3, "Copy() failed!");

        //Cleanup
        const auto res4 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res4, "Copy() failed!");
    }

    //Rename (File)
    TP_INFO("Rename (file)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Rename() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Rename() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Rename("test/test.txt", "\\/:*?\"<>|.txt");
        TRAP_ASSERT(!res2, "Rename() failed!");

        //Check
        TRAP_ASSERT(!std::filesystem::exists("test/\\/:*?\"<>|.txt"), "Rename() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "Rename() failed!");
    }

    //Rename (Folder)
    TP_INFO("Rename (folder)...");
    {
        //Setup
        const auto res = TRAP::FileSystem::CreateFolder("test");
        TRAP_ASSERT(res, "Rename() failed!");
        const auto res1 = TRAP::FileSystem::WriteTextFile("test/test.txt", "Some stupid text", TRAP::FileSystem::WriteMode::Overwrite);
        TRAP_ASSERT(res1, "Rename() failed!");

        //Test
        const auto res2 = TRAP::FileSystem::Rename("test", "\\/:*?\"<>|");
        TRAP_ASSERT(!res2, "Rename() failed!");

        //Check
        TRAP_ASSERT(std::filesystem::exists("test"), "Rename() failed!");

        //Cleanup
        const auto res3 = TRAP::FileSystem::Delete("test");
        TRAP_ASSERT(res3, "Rename() failed!");
    }

    //Exists
    TP_INFO("Exists...");
    {
#ifdef TRAP_PLATFORM_WINDOWS
        //Test
        //Folder with characters that are not allowed on Windows/NTFS
        const std::string path = "\\/:*?\"<>|";
        const auto res = TRAP::FileSystem::Exists(path);
        TRAP_ASSERT(!res, "Exists() failed!");
#endif /*TRAP_PLATFORM_WINDOWS*/
    }

    //GetSize (File)
    TP_INFO("GetSize (file)...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetSize("Some/Invalid/File.txt");
        TRAP_ASSERT(!res, "GetSize() failed!");
    }

    //GetSize (Folder)
    TP_INFO("GetSize (folder)...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetSize("Some/Invalid/Folder", false);
        TRAP_ASSERT(!res, "GetSize() failed!");
    }

    //GetLastWriteTime (File)
    TP_INFO("GetLastWriteTime (file)...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetLastWriteTime("Some/Invalid/File.txt");
        TRAP_ASSERT(!res, "GetLastWriteTime() failed!");
    }

    //GetLastWriteTime (Folder)
    TP_INFO("GetLastWriteTime (folder)...");
    {
        //Test
        const auto res = TRAP::FileSystem::GetLastWriteTime("Some/Invalid/Folder");
        TRAP_ASSERT(!res, "GetLastWriteTime() failed!");
    }

    //GetFolderPath
    TP_INFO("GetFolderPath...");
    {
        //Test
        const auto res1 = TRAP::FileSystem::GetFolderPath("test.txt");
        TRAP_ASSERT(!res1, "GetFolderPath() failed!");
    }
}
