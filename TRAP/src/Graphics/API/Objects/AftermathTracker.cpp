#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/String/String.h"

namespace
{
    bool AftermathInitialized = false;

#ifdef ENABLE_NSIGHT_AFTERMATH
    void* AftermathHandle = nullptr;
    PFN_GFSDK_Aftermath_EnableGpuCrashDumps AftermathEnableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_DisableGpuCrashDumps AftermathDisableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_GetCrashDumpStatus AftermathGetGPUCrashDumpStatus = nullptr;
#endif /*ENABLE_NSIGHT_AFTERMATH*/

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
    void OnGPUCrashDump([[maybe_unused]] const void* const gpuCrashDump,
                        [[maybe_unused]] const u32 gpuCrashDumpSize,
                        [[maybe_unused]] void* const userData)
    {
        ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

        TRAP_ASSERT(gpuCrashDump, "OnGPUCrashDump(): gpuCrashDump is nullptr!");
        TRAP_ASSERT(gpuCrashDumpSize, "OnGPUCrashDump(): gpuCrashDumpSize is 0!");

#ifndef TRAP_HEADLESS_MODE
        const auto targetFolder = TRAP::FileSystem::GetGameDocumentsFolderPath();
#else
        const auto targetFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
#endif
        if(!targetFolder)
            return;

        std::string dateTimeStamp = TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now());
        std::ranges::replace(dateTimeStamp, ':', '-');

        const std::filesystem::path folderPath = *targetFolder / "crash-dumps";
        const std::filesystem::path filePath = folderPath / fmt::format("crash_{}.nv-gpudmp", dateTimeStamp);

        if(!TRAP::FileSystem::Exists(folderPath))
            TRAP::FileSystem::CreateFolder(folderPath);
        TRAP::FileSystem::WriteFile(filePath, {static_cast<const u8*>(gpuCrashDump), gpuCrashDumpSize});
    }
#endif /*ENABLE_NSIGHT_AFTERMATH*/

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
    bool LoadFunctions()
    {
        ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
    }
#endif

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
    void UnloadFunctions()
    {
        ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

        if(AftermathHandle != nullptr)
            TRAP::Utils::DynamicLoading::FreeLibrary(AftermathHandle);

        AftermathHandle = nullptr;
        AftermathEnableGPUCrashDumps = nullptr;
        AftermathDisableGPUCrashDumps = nullptr;
        AftermathGetGPUCrashDumpStatus = nullptr;
    }
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[maybe_unused]] bool TRAP::Graphics::AftermathTracker::Initialize()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(!AftermathInitialized)
        return;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef ENABLE_NSIGHT_AFTERMATH
    AftermathCall(AftermathDisableGPUCrashDumps());

    UnloadFunctions();
#endif /*ENABLE_NSIGHT_AFTERMATH*/

    AftermathInitialized = false;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH

[[nodiscard]] GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(!AftermathInitialized)
        return GFSDK_Aftermath_Result_FAIL_NotInitialized;

    return AftermathGetGPUCrashDumpStatus(&outStatus);
}

#endif /*ENABLE_NSIGHT_AFTERMATH*/
