#include "TRAPPCH.h"
#include "DynamicLoading.h"

void* TRAP::Utils::DynamicLoading::LoadLibrary(const std::string_view path)
{
	ZoneScopedC(tracy::Color::Violet);

#ifdef TRAP_PLATFORM_WINDOWS
    return LoadLibraryA(path.data());
#elif defined(TRAP_PLATFORM_LINUX)
	return dlopen(path.data(), RTLD_LAZY | RTLD_LOCAL);
#else
    return nullptr;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::DynamicLoading::FreeLibrary(void* const module)
{
	ZoneScopedC(tracy::Color::Violet);

#ifdef TRAP_PLATFORM_WINDOWS
	FreeLibrary(static_cast<HMODULE>(module));
#elif defined(TRAP_PLATFORM_LINUX)
    dlclose(module);
#endif
}
