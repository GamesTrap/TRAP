#include "TRAPPCH.h"
#include "DynamicLoading.h"

[[nodiscard]] void* TRAP::Utils::DynamicLoading::LoadLibrary(const std::string_view path)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

#ifdef TRAP_PLATFORM_WINDOWS
	HMODULE handle = LoadLibraryA(path.data());
	if(!handle)
	{
		TP_ERROR(Log::UtilsPrefix, "Failed to load library: ", path);
		TP_ERROR(Log::UtilsPrefix, Utils::String::GetStrError());
	}
    return handle;
#elif defined(TRAP_PLATFORM_LINUX)

#if defined(__SANITIZE_LEAK__) || __has_feature(leak_sanitizer) || defined(TRAP_LSAN)
	//Workaround: This fixes false positives in LeakSanitizer
	static constexpr int32_t mode = RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE;
#else
	static constexpr int32_t mode = RTLD_LAZY | RTLD_LOCAL;
#endif

	void* handle = dlopen(path.data(), mode);
	if(handle == nullptr)
	{
		TP_ERROR(Log::UtilsPrefix, "Failed to load library: ", path);
		TP_ERROR(Log::UtilsPrefix, dlerror());
	}
	return handle;
#else
    return nullptr;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::DynamicLoading::FreeLibrary(void* const module)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

#ifdef TRAP_PLATFORM_WINDOWS
	if(!FreeLibrary(static_cast<HMODULE>(module)))
	{
		TP_ERROR(Log::UtilsPrefix, "Failed to free library!");
		TP_ERROR(Log::UtilsPrefix, Utils::String::GetStrError());
	}
#elif defined(TRAP_PLATFORM_LINUX)
    if(dlclose(module) != 0)
	{
		TP_ERROR(Log::UtilsPrefix, "Failed to free library!");
		TP_ERROR(Log::UtilsPrefix, dlerror());
	}
#endif
}
