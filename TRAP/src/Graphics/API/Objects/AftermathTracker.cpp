#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Application.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

bool AftermathInitialized = false;

TracyLockable(std::mutex, AftermathMutex);

void* AftermathHandle = nullptr;
#ifdef ENABLE_NSIGHT_AFTERMATH
PFN_GFSDK_Aftermath_EnableGpuCrashDumps AftermathEnableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_DisableGpuCrashDumps AftermathDisableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_GetCrashDumpStatus AftermathGetGPUCrashDumpStatus = nullptr;
#endif /*ENABLE_NSIGHT_AFTERMATH*/

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
    std::replace(dateTimeStamp.begin(), dateTimeStamp.end(), ':', '-');

    const std::filesystem::path folderPath = *targetFolder / "crash-dumps";
    const std::filesystem::path filePath = folderPath / fmt::format("crash_{}.nv-gpudmp", dateTimeStamp);
    std::lock_guard lock(AftermathMutex);
    LockMark(AftermathMutex);
    std::vector<uint8_t> buffer(gpuCrashDumpSize);
    std::copy_n(static_cast<const uint8_t*>(gpuCrashDump), gpuCrashDumpSize, buffer.begin());
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

void TRAP::Graphics::AftermathTracker::SetAftermathMarker([[maybe_unused]] const std::string_view name) noexcept
{
#ifdef ENABLE_NSIGHT_AFTERMATH
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH

void TRAP::Graphics::AftermathTracker::AftermathCall(const GFSDK_Aftermath_Result res)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(outStatus, "AftermathTracker::GetGPUCrashDumpStatus(): outStatus is nullptr!");

    if(!AftermathInitialized)
        return GFSDK_Aftermath_Result_FAIL_NotInitialized;

    return AftermathGetGPUCrashDumpStatus(&outStatus);
}

#endif /*ENABLE_NSIGHT_AFTERMATH*/
