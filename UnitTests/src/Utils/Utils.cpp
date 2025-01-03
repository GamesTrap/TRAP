#include <catch2/catch_test_macros.hpp>

#include "Utils/Utils.h"
#include "Utils/Win.h"

TEST_CASE("TRAP::Utils::StringHasher", "[utils][stringhasher]")
{
    STATIC_REQUIRE(std::same_as<TRAP::Utils::StringHasher::is_transparent, void>);

    REQUIRE(TRAP::Utils::StringHasher{}("Hello World") > 0u);
}

TEST_CASE("TRAP::Utils", "[utils]")
{
    SECTION("UnorderedStringMap")
    {
        STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::key_type, std::string>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::mapped_type, u32>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::hasher, TRAP::Utils::StringHasher>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::UnorderedStringMap<u32>::key_equal, std::equal_to<>>);
    }

    SECTION("GetEndian()")
    {
        STATIC_REQUIRE(TRAP::Utils::GetEndian() == static_cast<TRAP::Utils::Endian>(std::endian::native == std::endian::little));
    }

    SECTION("AsBytes()")
    {
        static constexpr std::array<u32, 5u> arr{100000u, 100001u, 100002u, 100003u, 100004u};
        static constexpr std::span arrSpan(arr);

        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsBytes(arrSpan))::element_type, const u8>);
        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsBytes(arrSpan))::value_type, u8>);
        STATIC_REQUIRE(decltype(TRAP::Utils::AsBytes(arrSpan))::extent == (5u * sizeof(decltype(arr)::value_type)));
        REQUIRE(TRAP::Utils::AsBytes(arrSpan).size() == (5u * sizeof(decltype(arr)::value_type)));

        const std::vector<u32> vec{100000u, 100001u, 100002u, 100003u, 100004u};
        const std::span vecSpan(vec);

        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsBytes(vecSpan))::element_type, const u8>);
        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsBytes(vecSpan))::value_type, u8>);
        STATIC_REQUIRE(decltype(TRAP::Utils::AsBytes(vecSpan))::extent == std::dynamic_extent);
        REQUIRE(TRAP::Utils::AsBytes(vecSpan).size() == (5u * sizeof(decltype(vec)::value_type)));
    }

    SECTION("AsWritableBytes()")
    {
        std::array<u32, 5u> arr{100000u, 100001u, 100002u, 100003u, 100004u};
        std::span arrSpan(arr);

        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsWritableBytes(arrSpan))::element_type, u8>);
        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsWritableBytes(arrSpan))::value_type, u8>);
        STATIC_REQUIRE(decltype(TRAP::Utils::AsWritableBytes(arrSpan))::extent == (5u * sizeof(decltype(arr)::value_type)));
        REQUIRE(TRAP::Utils::AsWritableBytes(arrSpan).size() == (5u * sizeof(decltype(arr)::value_type)));

        std::vector<u32> vec{100000u, 100001u, 100002u, 100003u, 100004u};
        std::span vecSpan(vec);

        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsWritableBytes(vecSpan))::element_type, u8>);
        STATIC_REQUIRE(std::same_as<decltype(TRAP::Utils::AsWritableBytes(vecSpan))::value_type, u8>);
        STATIC_REQUIRE(decltype(TRAP::Utils::AsWritableBytes(vecSpan))::extent == std::dynamic_extent);
        REQUIRE(TRAP::Utils::AsWritableBytes(vecSpan).size() == (5u * sizeof(decltype(vec)::value_type)));
    }

    SECTION("IsStdSpan")
    {
        STATIC_REQUIRE_FALSE(TRAP::Utils::IsStdSpan<std::array<u32, 5u>>::value);
        STATIC_REQUIRE_FALSE(TRAP::Utils::IsStdSpan<std::vector<u32>>::value);
        STATIC_REQUIRE(TRAP::Utils::IsStdSpan<std::span<u32, 5u>>::value);
        STATIC_REQUIRE(TRAP::Utils::IsStdSpan<std::span<const u32, 5u>>::value);
        STATIC_REQUIRE(TRAP::Utils::IsStdSpan<std::span<const u32, std::dynamic_extent>>::value);
        STATIC_REQUIRE(TRAP::Utils::IsStdSpan<std::span<u32, std::dynamic_extent>>::value);
    }

    SECTION("GetCPUInfo()")
    {
        const TRAP::Utils::CPUInfo& cpuInfo = TRAP::Utils::GetCPUInfo();

        REQUIRE_FALSE(cpuInfo.Model.empty());
        REQUIRE(cpuInfo.Cores > 0u);
        REQUIRE(cpuInfo.LogicalCores > 0u);
    }

    SECTION("GetLinuxWindowManager()")
    {
#ifdef TRAP_PLATFORM_LINUX
        // [[maybe_unused]] const TRAP::Utils::LinuxWindowManager wm = TRAP::Utils::GetLinuxWindowManager();
#else
        REQUIRE(TRAP::Utils::GetLinuxWindowManager() == TRAP::Utils::LinuxWindowManager::Unknown);
#endif
    }

#ifdef TRAP_PLATFORM_WINDOWS
    SECTION("IsWindowsVersionOrGreaterWin32()")
    {
        REQUIRE(TRAP::Utils::IsWindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0u));
    }

    SECTION("IsWindows10BuildOrGreaterWin32()")
    {
        REQUIRE(TRAP::Utils::IsWindows10BuildOrGreaterWin32(10240));
    }

    SECTION("IsWindows11BuildOrGreaterWin32()")
    {
        [[maybe_unused]] const bool res = TRAP::Utils::IsWindows11BuildOrGreaterWin32(22000);
    }

    SECTION("IsWindows11OrGreaterWin32()")
    {
        [[maybe_unused]] const bool res = TRAP::Utils::IsWindows11OrGreaterWin32();
    }

    SECTION("IsWindows10Version1607OrGreaterWin32()")
    {
        [[maybe_unused]] const bool res = TRAP::Utils::IsWindows10Version1607OrGreaterWin32();
    }

    SECTION("IsWindows10Version1703OrGreaterWin32()")
    {
        [[maybe_unused]] const bool res = TRAP::Utils::IsWindows10Version1703OrGreaterWin32();
    }

    SECTION("IsWindows10OrGreaterWin32()")
    {
        REQUIRE(TRAP::Utils::IsWindows10OrGreaterWin32());
    }
#endif /*TRAP_PLATFORM_WINDOWS*/

    SECTION("fmt specialization TRAP::Utils::LinuxWindowManager")
    {
        REQUIRE(fmt::format("{}", TRAP::Utils::LinuxWindowManager::Unknown) == "Unknown");
        REQUIRE(fmt::format("{}", TRAP::Utils::LinuxWindowManager::X11) == "X11");
        REQUIRE(fmt::format("{}", TRAP::Utils::LinuxWindowManager::Wayland) == "Wayland");
        REQUIRE(fmt::format("{}", static_cast<TRAP::Utils::LinuxWindowManager>(10)) == "<MISSING ENUM VALUE>");
    }

    SECTION("ConvertToType<TRAP::Utils::LinuxWindowManager>()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("X11") == TRAP::Utils::LinuxWindowManager::X11);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("x11") == TRAP::Utils::LinuxWindowManager::X11);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("Wayland") == TRAP::Utils::LinuxWindowManager::Wayland);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("wayland") == TRAP::Utils::LinuxWindowManager::Wayland);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("WAYLAND") == TRAP::Utils::LinuxWindowManager::Wayland);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("wAyLaNd") == TRAP::Utils::LinuxWindowManager::Wayland);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("ababababbaba") == TRAP::Utils::LinuxWindowManager::Unknown);
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<TRAP::Utils::LinuxWindowManager>("") == TRAP::Utils::LinuxWindowManager::Unknown);
    }
}
