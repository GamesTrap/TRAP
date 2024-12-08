#include <catch2/catch_test_macros.hpp>

#include "Core/PlatformDetection.h"
#include "Core/Base.h"

#ifdef TRAP_PLATFORM_WINDOWS
#include "Utils/Win.h"
#elif defined(TRAP_PLATFORM_LINUX)
#include "Utils/Linux.h"
#endif

#include "Utils/DynamicLoading/DynamicLoading.h"

TEST_CASE("TRAP::Utils::DynamicLoading", "[utils][dynamicloading]")
{
    SECTION("LoadLibrary()")
    {
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::LoadLibrary(""));
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::LoadLibrary("non_existing"));

#ifdef TRAP_PLATFORM_LINUX
        void* libc = TRAP::Utils::DynamicLoading::LoadLibrary("libc.so.6"); //Shared library should be included with every Linux installation
        REQUIRE(libc);
        TRAP::Utils::DynamicLoading::FreeLibrary(libc);
#elif defined(TRAP_PLATFORM_WINDOWS)
        void* kernel32 = TRAP::Utils::DynamicLoading::LoadLibrary("kernel32.dll"); //DLL should be included with every Windows installation
        REQUIRE(kernel32);
        TRAP::Utils::DynamicLoading::FreeLibrary(kernel32);
#endif
    }

    SECTION("FreeLibrary()")
    {
        TRAP::Utils::DynamicLoading::FreeLibrary(nullptr);

#ifdef TRAP_PLATFORM_LINUX
        void* const libc = TRAP::Utils::DynamicLoading::LoadLibrary("libc.so.6");
        TRAP::Utils::DynamicLoading::FreeLibrary(libc);
#elif defined(TRAP_PLATFORM_WINDOWS)
        void* const kernel32 = TRAP::Utils::DynamicLoading::LoadLibrary("kernel32.dll");
        TRAP::Utils::DynamicLoading::FreeLibrary(kernel32);
#endif
    }

    SECTION("GetLibrarySymbol()")
    {
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(nullptr, "some_non-existing_function"));
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(nullptr, ""));

#ifdef TRAP_PLATFORM_LINUX
        void* const libc = TRAP::Utils::DynamicLoading::LoadLibrary("libc.so.6");

        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(libc, ""));
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(libc, "some_non-existing_function"));
        REQUIRE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<int(*)(const char*, ...)>(libc, "printf")); //Try to load printf() from libc.so

        TRAP::Utils::DynamicLoading::FreeLibrary(libc);
#elif defined(TRAP_PLATFORM_WINDOWS)
        void* const kernel32 = TRAP::Utils::DynamicLoading::LoadLibrary("kernel32.dll");

        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(kernel32, ""));
        REQUIRE_FALSE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<void(*)()>(kernel32, "some_non-existing_function"));
        REQUIRE(TRAP::Utils::DynamicLoading::GetLibrarySymbol<DWORD(*)()>(kernel32, "GetTickCount")); //Try to load GetTickCount() from kernel32.dll

        TRAP::Utils::DynamicLoading::FreeLibrary(kernel32);
#endif
    }
}
