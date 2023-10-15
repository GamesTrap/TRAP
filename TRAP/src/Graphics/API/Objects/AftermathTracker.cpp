#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/String/String.h"

namespace
{
    bool AftermathInitialized = false;

#ifdef ENABLE_NSIGHT_AFTERMATH
    TracyLockable(std::mutex, AftermathMutex);
    void* AftermathHandle = nullptr;
    PFN_GFSDK_Aftermath_EnableGpuCrashDumps AftermathEnableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_DisableGpuCrashDumps AftermathDisableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_GetCrashDumpStatus AftermathGetGPUCrashDumpStatus = nullptr;
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void OnGPUCrashDump([[maybe_unused]] const void* gpuCrashDump,
                    [[maybe_unused]] const uint32_t gpuCrashDumpSize,
                    [[maybe_unused]] void* userData)
{
#ifdef ENABLE_NSIGHT_AFTERMATH
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(gpuCrashDump, "OnGPUCrashDump(): gpuCrashDump is nullptr!");
    TRAP_ASSERT(gpuCrashDumpSize, "OnGPUCrashDump(): gpuCrashDumpSize is 0!");

#ifndef TRAP_HEADLESS_MODE
    const auto targetFolder = TRAP::FileSystem::GetGameDocumentsFolderPath();
#else
    const auto targetFolder = TRAP::FileSystem::GetCurrentFolderPath();
#endif
    if(!targetFolder)
        return;

    std::string dateTimeStamp = TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now());
    std::ranges::replace(dateTimeStamp, ':', '-');

    const std::filesystem::path folderPath = *targetFolder / "crash-dumps";
    const std::filesystem::path filePath = folderPath / fmt::format("crash_{}.nv-gpudmp", dateTimeStamp);
    std::lock_guard lock(AftermathMutex);
    LockMark(AftermathMutex);

    const std::vector<uint8_t> buffer(static_cast<const uint8_t*>(gpuCrashDump),
                                      static_cast<const uint8_t*>(gpuCrashDump) + gpuCrashDumpSize);
    if(!TRAP::FileSystem::Exists(folderPath))
        TRAP::FileSystem::CreateFolder(folderPath);
    TRAP::FileSystem::WriteFile(filePath, buffer);
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

bool LoadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef TRAP_PLATFORM_WINDOWS
    AftermathHandle = TRAP::Utils::DynamicLoading::LoadLibrary("GFSDK_Aftermath_Lib.x64.dll");
#elif defined(TRAP_PLATFORM_LINUX)
    AftermathHandle = TRAP::Utils::DynamicLoading::LoadLibrary("libGFSDK_Aftermath_Lib.x64.so");
#endif

    if(AftermathHandle == nullptr)
        return false;

    AftermathEnableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_EnableGpuCrashDumps>
    (
        AftermathHandle, "GFSDK_Aftermath_EnableGpuCrashDumps"
    );
    AftermathDisableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_DisableGpuCrashDumps>
    (
        AftermathHandle, "GFSDK_Aftermath_DisableGpuCrashDumps"
    );
    AftermathGetGPUCrashDumpStatus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_GetCrashDumpStatus>
    (
        AftermathHandle, "GFSDK_Aftermath_GetCrashDumpStatus"
    );

    return (AftermathEnableGPUCrashDumps != nullptr) && (AftermathDisableGPUCrashDumps != nullptr) && (AftermathGetGPUCrashDumpStatus != nullptr);
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void UnloadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(AftermathHandle != nullptr)
        TRAP::Utils::DynamicLoading::FreeLibrary(AftermathHandle);

    AftermathHandle = nullptr;
    AftermathEnableGPUCrashDumps = nullptr;
    AftermathDisableGPUCrashDumps = nullptr;
    AftermathGetGPUCrashDumpStatus = nullptr;
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[maybe_unused]] bool TRAP::Graphics::AftermathTracker::Initialize()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(AftermathInitialized)
        return true;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Creating AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef ENABLE_NSIGHT_AFTERMATH
    if(!LoadFunctions())
        return false;

    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
    {
        AftermathInitialized = true;

        AftermathCall(AftermathEnableGPUCrashDumps(GFSDK_Aftermath_Version_API,
                                                   GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan,
                                                   GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default,
                                                   OnGPUCrashDump, nullptr, nullptr, nullptr, nullptr));
    }

#endif /*ENABLE_NSIGHT_AFTERMATH*/

    return AftermathInitialized;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(!AftermathInitialized)
        return;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef ENABLE_NSIGHT_AFTERMATH
    AftermathCall(AftermathDisableGPUCrashDumps());

    UnloadFunctions();
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH

[[nodiscard]] GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(!AftermathInitialized)
        return GFSDK_Aftermath_Result_FAIL_NotInitialized;

    return AftermathGetGPUCrashDumpStatus(&outStatus);
}

#endif /*ENABLE_NSIGHT_AFTERMATH*/
