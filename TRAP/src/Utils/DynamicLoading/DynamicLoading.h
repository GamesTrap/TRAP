#ifndef TRAP_DYNAMICLOADING_H
#define TRAP_DYNAMICLOADING_H

#include <string>

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
#ifdef TRAP_PLATFORM_WINDOWS
    return reinterpret_cast<T>(::GetProcAddress(static_cast<HMODULE>(module), name.data()));
#elif defined(TRAP_PLATFORM_LINUX)
    return reinterpret_cast<T>(dlsym(module, name.data()));
#else
    static_assert(false, "GetLibrarySymbol() not implemented for this platform!");
    return T();
#endif
}

#endif /*TRAP_DYNAMICLOADING_H*/