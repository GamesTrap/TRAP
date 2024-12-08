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
    /// @return Pointer to the loaded library on success, nullptr otherwise.
    /// @threadsafe
    [[nodiscard]] void* LoadLibrary(const std::string& path);
    /// @brief Unloads a dynamic library from memory.
    /// @param libraryHandle Pointer to the library to unload.
    /// @threadsafe
    void FreeLibrary(void* libraryHandle);
    /// @brief Gets a function pointer from a dynamic library.
    /// @param libraryHandle Pointer to the library.
    /// @param name Name of the function.
    /// @return Pointer to the function on success, nullptr otherwise.
    /// @threadsafe
    template<typename T>
    [[nodiscard]] T GetLibrarySymbol(void* libraryHandle, const std::string& name);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] inline T TRAP::Utils::DynamicLoading::GetLibrarySymbol([[maybe_unused]] void* const libraryHandle, [[maybe_unused]] const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    if(libraryHandle == nullptr || name.empty())
        return T();

#ifdef TRAP_PLATFORM_WINDOWS
    void* proc = ::GetProcAddress(static_cast<HMODULE>(libraryHandle), name.c_str());
    if(!proc)
    {
        TP_ERROR(Log::UtilsPrefix, "Failed to get symbol: ", name);
        TP_ERROR(Log::UtilsPrefix, Utils::String::GetStrError());
    }
    return reinterpret_cast<T>(proc);
#elif defined(TRAP_PLATFORM_LINUX)
    void* const symbol = dlsym(libraryHandle, name.c_str());
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
