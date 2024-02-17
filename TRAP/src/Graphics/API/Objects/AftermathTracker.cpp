#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/String/String.h"

namespace
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    PFN_GFSDK_Aftermath_EnableGpuCrashDumps AftermathEnableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_DisableGpuCrashDumps AftermathDisableGPUCrashDumps = nullptr;
    PFN_GFSDK_Aftermath_GetCrashDumpStatus AftermathGetGPUCrashDumpStatus = nullptr;

    void AftermathHandleDeleter(void* aftermathHandle)
    {
        TRAP::Utils::DynamicLoading::FreeLibrary(aftermathHandle);

        AftermathEnableGPUCrashDumps = nullptr;
        AftermathDisableGPUCrashDumps = nullptr;
        AftermathGetGPUCrashDumpStatus = nullptr;
    }
    TRAP::UniqueResource<void*, decltype(&AftermathHandleDeleter)> AftermathHandle{};
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
        AftermathHandle = TRAP::MakeUniqueResourceChecked(TRAP::Utils::DynamicLoading::LoadLibrary("GFSDK_Aftermath_Lib.x64.dll"), nullptr, &AftermathHandleDeleter);
#elif defined(TRAP_PLATFORM_LINUX)
        AftermathHandle = TRAP::MakeUniqueResourceChecked(TRAP::Utils::DynamicLoading::LoadLibrary("libGFSDK_Aftermath_Lib.x64.so"), nullptr, &AftermathHandleDeleter);
#endif

        if(AftermathHandle.Get() == nullptr)
            return false;

        AftermathEnableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_EnableGpuCrashDumps>
        (
            AftermathHandle.Get(), "GFSDK_Aftermath_EnableGpuCrashDumps"
        );
        AftermathDisableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_DisableGpuCrashDumps>
        (
            AftermathHandle.Get(), "GFSDK_Aftermath_DisableGpuCrashDumps"
        );
        AftermathGetGPUCrashDumpStatus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_GetCrashDumpStatus>
        (
            AftermathHandle.Get(), "GFSDK_Aftermath_GetCrashDumpStatus"
        );

        return (AftermathEnableGPUCrashDumps != nullptr) && (AftermathDisableGPUCrashDumps != nullptr) && (AftermathGetGPUCrashDumpStatus != nullptr);
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[maybe_unused]] bool TRAP::Graphics::AftermathTracker::Initialize()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_NSIGHT_AFTERMATH
    if(AftermathHandle.Get() != nullptr)
        return true;

#ifdef ENABLE_GRAPHICS_DEBUG
    TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Creating AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

    if(!LoadFunctions())
    {
        AftermathHandle.Reset();
        return false;
    }

    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
    {
        AftermathCall(AftermathEnableGPUCrashDumps(GFSDK_Aftermath_Version_API,
                                                   GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan,
                                                   GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default,
                                                   OnGPUCrashDump, nullptr, nullptr, nullptr, nullptr));
    }

#endif /*ENABLE_NSIGHT_AFTERMATH*/

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
    if(AftermathHandle.Get() == nullptr)
        return;

    TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef ENABLE_NSIGHT_AFTERMATH
    AftermathCall(AftermathDisableGPUCrashDumps());

    AftermathHandle.Reset();
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH

[[nodiscard]] GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(AftermathHandle.Get() == nullptr)
        return GFSDK_Aftermath_Result_FAIL_NotInitialized;

    return AftermathGetGPUCrashDumpStatus(&outStatus);
}

#endif /*ENABLE_NSIGHT_AFTERMATH*/
