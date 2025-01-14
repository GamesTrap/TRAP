#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/String/String.h"
#include "Graphics/API/RendererAPI.h"

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

#ifdef ENABLE_NSIGHT_AFTERMATH
    if(AftermathHandle.Get() == nullptr)
        return;

#ifdef ENABLE_GRAPHICS_DEBUG
    TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif /*ENABLE_GRAPHICS_DEBUG*/

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

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
void TRAP::Graphics::AftermathTracker::INTERNAL::AftermathCallImpl(const GFSDK_Aftermath_Result res)
{
    if(res == GFSDK_Aftermath_Result_Success)
        return;

    switch(res)
    {
    case GFSDK_Aftermath_Result_FAIL_VersionMismatch:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Version mismatch (wrong .dll/.so version?");
        break;

    case GFSDK_Aftermath_Result_FAIL_NotInitialized:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Not initialized");
        break;

    case GFSDK_Aftermath_Result_FAIL_InvalidAdapter:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Invalid adapter (only NVIDIA GPUs are supported)");
        break;

    case GFSDK_Aftermath_Result_FAIL_InvalidParameter:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Invalid parameter (nullptr or bad handle)");
        break;

    case GFSDK_Aftermath_Result_FAIL_ApiError:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "API error");
        break;

    case GFSDK_Aftermath_Result_FAIL_NvApiIncompatible:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "NvAPI incompatible (NvAPI not up to date?)");
        break;

    case GFSDK_Aftermath_Result_FAIL_GettingContextDataWithNewCommandList:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Getting context data with new command list");
        break;

    case GFSDK_Aftermath_Result_FAIL_AlreadyInitialized:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Already initialized");
        break;

    case GFSDK_Aftermath_Result_FAIL_D3DDebugLayerNotCompatible:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "D3D Debug Layer not compatible");
        break;

    case GFSDK_Aftermath_Result_FAIL_DriverInitFailed:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Driver init failed");
        break;

    case GFSDK_Aftermath_Result_FAIL_DriverVersionNotSupported:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Driver version not supported (requires NVIDIA driver 387.xx or newer)");
        break;

    case GFSDK_Aftermath_Result_FAIL_OutOfMemory:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Out of memory");
        break;

    case GFSDK_Aftermath_Result_FAIL_GetDataOnBundle:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Get data on bundle");
        break;

    case GFSDK_Aftermath_Result_FAIL_GetDataOnDeferredContext:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Get data on deferred context");
        break;

    case GFSDK_Aftermath_Result_FAIL_FeatureNotEnabled:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Feature not enabled (missing feature flag?)");
        break;

    case GFSDK_Aftermath_Result_FAIL_NoResourcesRegistered:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "No resources registered");
        break;

    case GFSDK_Aftermath_Result_FAIL_ThisResourceNeverRegistered:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "This resource never registered");
        break;

    case GFSDK_Aftermath_Result_FAIL_NotSupportedInUWP:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Not supported in UWP");
        break;

    case GFSDK_Aftermath_Result_FAIL_D3dDllNotSupported:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "D3D DLL not supported");
        break;

    case GFSDK_Aftermath_Result_FAIL_D3dDllInterceptionNotSupported:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "D3D DLL interception not supported");
        break;

    case GFSDK_Aftermath_Result_FAIL_Disabled:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Disabled by user");
        break;

    case GFSDK_Aftermath_Result_FAIL_Unknown:
        [[fallthrough]];
    default:
        TP_ERROR(Log::RendererAftermathTrackerPrefix, "Unknown AftermathTracker error");
        break;
    }
}
#endif /*ENABLE_NSIGHT_AFTERMATH*/
