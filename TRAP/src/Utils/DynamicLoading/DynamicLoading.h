#ifndef TRAP_DYNAMICLOADING_H
#define TRAP_DYNAMICLOADING_H

#include <string>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "Utils/Linux.h"
#include "Log/Log.h"

namespace TRAP::Utils::DynamicLoading
{
    /// @brief Loads a dynamic library into memory.
    /// @param path Path to the library.
    /// @return Pointer to the loaded library.
    [[nodiscard]] void* LoadLibrary(std::string_view path);
    /// @brief Unloads a dynamic library from memory.
    /// @param module Pointer to the library to unload.
    void FreeLibrary(void* module);
    /// @brief Gets a function pointer from a dynamic library.
    /// @param module Pointer to the library.
    /// @param name Name of the function.
    /// @return Pointer to the function.
    template<typename T>
    [[nodiscard]] T GetLibrarySymbol(void* module, std::string_view name);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] inline T TRAP::Utils::DynamicLoading::GetLibrarySymbol([[maybe_unused]] void* module, [[maybe_unused]] const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

#ifdef TRAP_PLATFORM_WINDOWS
    FARPROC proc = ::GetProcAddress(static_cast<HMODULE>(module), name.data());
    if(!proc)
    {
        TP_ERROR(Log::UtilsPrefix, "Failed to get symbol: ", name);
        TP_ERROR(Log::UtilsPrefix, Utils::String::GetStrError());
    }
    return reinterpret_cast<T>(proc);
#elif defined(TRAP_PLATFORM_LINUX)
    void* symbol = dlsym(module, name.data());
    if(!symbol)
    {
        TP_ERROR(Log::UtilsPrefix, "Failed to get symbol: ", name);
        TP_ERROR(Log::UtilsPrefix, dlerror());
    }
    return reinterpret_cast<T>(symbol);
#else
    static_assert(false, "GetLibrarySymbol() not implemented for this platform!");
    return T();
#endif
}

#endif /*TRAP_DYNAMICLOADING_H*/