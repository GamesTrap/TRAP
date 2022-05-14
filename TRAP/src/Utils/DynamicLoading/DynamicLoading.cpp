#include "TRAPPCH.h"
#include "DynamicLoading.h"

void* TRAP::Utils::DynamicLoading::LoadLibrary(const std::string& path)
{
#ifdef TRAP_PLATFORM_WINDOWS
    return LoadLibraryA(path.c_str());
#elif defined(TRAP_PLATFORM_LINUX)
	return dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::DynamicLoading::FreeLibrary(void* module)
{
#ifdef TRAP_PLATFORM_WINDOWS
	FreeLibrary(static_cast<HMODULE>(module));
#elif defined(TRAP_PLATFORM_LINUX)
    dlclose(module);
#endif
}
