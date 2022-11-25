#ifndef TRAP_DYNAMICLOADING_H
#define TRAP_DYNAMICLOADING_H

#include <string>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

namespace TRAP::Utils::DynamicLoading
{
    /// <summary>
    /// Loads a dynamic library into memory.
    /// </summary>
    /// <param name="path">Path to the library.</param>
    /// <returns>Pointer to the loaded library.</returns>
    void* LoadLibrary(std::string_view path);
    /// <summary>
    /// Unloads a dynamic library from memory.
    /// </summary>
    /// <param name="module">Pointer to the library to unload.</param>
    void FreeLibrary(void* module);
    /// <summary>
    /// Gets a function pointer from a dynamic library.
    /// </summary>
    /// <param name="module">Pointer to the library.</param>
    /// <param name="name">Name of the function.</param>
    /// <returns>Pointer to the function.</returns>
    template<typename T>
    T GetLibrarySymbol(void* module, const std::string_view name);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
inline T TRAP::Utils::DynamicLoading::GetLibrarySymbol([[maybe_unused]] void* module, [[maybe_unused]] const std::string_view name)
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