#include "Catch2FmtSupport.h"

#include "Utils/Config/Config.h"
#include "FileSystem/FileSystem.h"

TEST_CASE("TRAP::Utils::Config", "[utils][config]")
{
    static const std::filesystem::path TestConfigFolderPath = "Testfiles/Utils/Config";
    static const std::filesystem::path TestConfigPath = TestConfigFolderPath / "testconfig.cfg";

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Config>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::Config>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::Config>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::Config config{};

        //Copy constructor
        // const TRAP::Utils::Config configCopy(config); //Deleted

        //Move constructor
        const TRAP::Utils::Config configMove(std::move(config));
    }

    SECTION("Assignments")
    {
        TRAP::Utils::Config config{};

        //Copy assignment operator
        // const TRAP::Utils::Config configCopy = config; //Deleted

        //Move assignment operator
        const TRAP::Utils::Config configMove = std::move(config);
    }

    SECTION("LoadFromFile()")
    {
        REQUIRE_FALSE(TRAP::Utils::Config{}.LoadFromFile(""));
        REQUIRE_FALSE(TRAP::Utils::Config{}.LoadFromFile("nonexistingfile.cfg"));

        REQUIRE(TRAP::Utils::Config{}.LoadFromFile(TestConfigFolderPath / "emptyconfig.cfg"));

        REQUIRE(TRAP::Utils::Config{}.LoadFromFile(TestConfigPath));
    }

    SECTION("SaveToFile()")
    {
        TRAP::Utils::Config config{};

        TRAP::FileSystem::Delete(TestConfigFolderPath / "savetofile.cfg");

        REQUIRE(config.LoadFromFile(TestConfigPath));
        REQUIRE(config.SaveToFile(TestConfigFolderPath / "savetofile.cfg"));

        REQUIRE(TRAP::FileSystem::ReadTextFile(TestConfigFolderPath / "savetofile_expected.cfg") == TRAP::FileSystem::ReadTextFile(TestConfigFolderPath / "savetofile.cfg"));

        REQUIRE(config.SaveToFile(TestConfigFolderPath / "savetofile.cfg"));

        REQUIRE(TRAP::FileSystem::ReadTextFile(TestConfigFolderPath / "savetofile_expected.cfg") == TRAP::FileSystem::ReadTextFile(TestConfigFolderPath / "savetofile.cfg"));

        TRAP::FileSystem::Delete(TestConfigFolderPath / "savetofile.cfg");
    }

    SECTION("HasChanged()")
    {
        TRAP::Utils::Config config{};

        REQUIRE_FALSE(config.HasChanged());

        config.Set("Str", "Hello World!");

        REQUIRE(config.HasChanged());
    }

    SECTION("Get()")
    {
        TRAP::Utils::Config config{};

        REQUIRE(config.LoadFromFile(TestConfigPath));

        REQUIRE(*config.Get<std::string>("Str") == "Value");
        REQUIRE(*config.Get<std::string>("STR") == "Value");
        REQUIRE(*config.Get<std::string>("str") == "Value");
        REQUIRE(*config.Get<i32>("Str") == 0);
        REQUIRE(*config.Get<i32>("Integer") == 100);
        REQUIRE(*config.Get<i32>("INTEGER") == 100);
        REQUIRE(*config.Get<i32>("integer") == 100);
        REQUIRE(*config.Get<std::string>("Integer") == "100");
        REQUIRE(*config.Get<i32>("NegativeInteger") == -10);
        REQUIRE(*config.Get<i32>("NEGATIVEINTEGER") == -10);
        REQUIRE(*config.Get<i32>("negativeinteger") == -10);
        REQUIRE(*config.Get<std::string>("NegativeInteger") == "-10");
        REQUIRE(*config.Get<bool>("Enable") == true);
        REQUIRE(*config.Get<bool>("ENABLE") == true);
        REQUIRE(*config.Get<bool>("enable") == true);
        REQUIRE(*config.Get<bool>("Disable") == false);
        REQUIRE(*config.Get<bool>("DISABLE") == false);
        REQUIRE(*config.Get<bool>("disable") == false);
        REQUIRE(*config.Get<f32>("Float") == 10.6f);
        REQUIRE(*config.Get<f32>("FLOAT") == 10.6f);
        REQUIRE(*config.Get<f32>("float") == 10.6f);
        REQUIRE(*config.Get<std::string>("List") == "hello,world,this engine rocks!");

        REQUIRE_FALSE(config.Get<std::string>(""));
        REQUIRE_FALSE(config.Get<std::string>("notexisting"));
    }

    SECTION("GetVector()")
    {
        TRAP::Utils::Config config{};

        REQUIRE(config.LoadFromFile(TestConfigPath));

        REQUIRE(*config.GetVector<std::string>("List") == std::vector<std::string>{"hello", "world", "this engine rocks!"});
        REQUIRE(*config.GetVector<std::string>("LIST") == std::vector<std::string>{"hello", "world", "this engine rocks!"});
        REQUIRE(*config.GetVector<std::string>("list") == std::vector<std::string>{"hello", "world", "this engine rocks!"});

        REQUIRE(*config.GetVector<std::string>("enable") == std::vector<std::string>{"true"});
        REQUIRE(*config.GetVector<i32>("integer") == std::vector<i32>{100});
        REQUIRE(*config.GetVector<f32>("float") == std::vector<f32>{10.6f});

        REQUIRE_FALSE(config.GetVector<std::string>(""));
        REQUIRE_FALSE(config.GetVector<std::string>("notexisting"));
    }

    SECTION("Set(key, val)")
    {
        TRAP::Utils::Config config{};

        config.Set("Str", "Value");
        config.Set("Integer", 100);
        config.Set("NegativeInteger", -10);
        config.Set("Enable", true);
        config.Set("Disable", false);
        config.Set("Float", 10.6f);

        REQUIRE(*config.Get<std::string>("Str") == "Value");
        REQUIRE(*config.Get<i32>("Integer") == 100);
        REQUIRE(*config.Get<i32>("NegativeInteger") == -10);
        REQUIRE(*config.Get<bool>("Enable"));
        REQUIRE_FALSE(*config.Get<bool>("Disable"));
        REQUIRE(*config.Get<f32>("Float") == 10.6f);

        config.Set("Str", "Other Value");

        REQUIRE(*config.Get<std::string>("Str") == "Other Value");
    }

    SECTION("Set(key, vector)")
    {
        TRAP::Utils::Config config{};

        config.Set("List", std::vector<std::string>{"hello", "world", "this engine rocks!"});

        REQUIRE(*config.GetVector<std::string>("List") == std::vector<std::string>{"hello", "world", "this engine rocks!"});

        config.Set("List", std::vector<std::string>{"new", "list"});

        REQUIRE(*config.GetVector<std::string>("List") == std::vector<std::string>{"new", "list"});
    }

    SECTION("Integration")
    {
        static const std::filesystem::path IntegrationTestConfigPath = TestConfigFolderPath / "integrationtest.cfg";
        static const std::filesystem::path IntegrationTestExpectedConfigPath = TestConfigFolderPath / "integrationtest_expected.cfg";

        REQUIRE(TRAP::FileSystem::Copy(TestConfigPath, IntegrationTestConfigPath, true));

        TRAP::Utils::Config config{};
        REQUIRE(config.LoadFromFile(IntegrationTestConfigPath));

        REQUIRE(*config.Get<i32>("Integer") == 100);
        config.Set("Integer", 200);
        REQUIRE(*config.Get<i32>("Integer") == 200);
        config.Set("NewValue", "Hello World");
        REQUIRE(*config.Get<std::string>("NewValue") == "Hello World");

        REQUIRE(config.SaveToFile(IntegrationTestConfigPath));
        REQUIRE(config.LoadFromFile(IntegrationTestConfigPath));

        REQUIRE(*config.Get<i32>("Integer") == 200);
        REQUIRE(*config.Get<std::string>("NewValue") == "Hello World");

        REQUIRE(TRAP::FileSystem::ReadTextFile(IntegrationTestExpectedConfigPath) == TRAP::FileSystem::ReadTextFile(IntegrationTestConfigPath));

        TRAP::FileSystem::Delete(IntegrationTestConfigPath);
    }
}
