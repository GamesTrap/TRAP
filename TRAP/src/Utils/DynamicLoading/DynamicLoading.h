#ifndef TRAP_DYNAMICLOADING_H
#define TRAP_DYNAMICLOADING_H

namespace TRAP::Utils::DynamicLoading
{
    /// <summary>
    /// Loads a dynamic library into memory.
    /// </summary>
    /// <param name="path">Path to the library.</param>
    /// <returns>Pointer to the loaded library.</returns>
    void* LoadLibrary(const std::string& path);
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
    T GetLibrarySymbol(void* module, const std::string& name);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
inline T TRAP::Utils::DynamicLoading::GetLibrarySymbol(void* module, const std::string& name)
{
#ifdef TRAP_PLATFORM_WINDOWS
    return reinterpret_cast<T>(::GetProcAddress(static_cast<HMODULE>(module), name.c_str()));
#elif defined(TRAP_PLATFORM_LINUX)
    return reinterpret_cast<T>(dlsym(module, name.c_str()));
#endif

    return T();
}

#endif /*TRAP_DYNAMICLOADING_H*/