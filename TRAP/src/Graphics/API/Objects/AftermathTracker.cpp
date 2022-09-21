#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FileSystem/FileSystem.h"
#include "Application.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

bool initialized = false;

std::mutex mutex{};

void* handle = nullptr;
#ifdef ENABLE_NSIGHT_AFTERMATH
#if defined(__d3d11_h__) || defined(__d3d12_h__)
GFSDK_Aftermath_ContextHandle aftermathHandle;
bool context = false;
PFN_GFSDK_Aftermath_DX12_Initialize dx12Initialize = nullptr;
PFN_GFSDK_Aftermath_DX12_CreateContextHandle dx12CreateContextHandle = nullptr;
PFN_GFSDK_Aftermath_ReleaseContextHandle releaseContextHandle = nullptr;
PFN_GFSDK_Aftermath_SetEventMarker setEventMarker = nullptr;
#endif
PFN_GFSDK_Aftermath_EnableGpuCrashDumps enableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_DisableGpuCrashDumps disableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_GetCrashDumpStatus getGPUCrashDumpStatus = nullptr;
#endif

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void OnCrashDump([[maybe_unused]] const void* gpuCrashDump,
                 [[maybe_unused]] const uint32_t gpuCrashDumpSize,
                 void* /*userData*/)
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    const auto docsFolder = TRAP::FileSystem::GetDocumentsFolderPath();
    if(!docsFolder)
        return;

    std::string dateTimeStamp = TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now());
    std::replace(dateTimeStamp.begin(), dateTimeStamp.end(), ':', '-');

    const std::filesystem::path folderPath = *docsFolder / "TRAP" / TRAP::Application::GetGameName() / "crash-dumps";
    const std::filesystem::path filePath = folderPath / ("crash_" + dateTimeStamp + ".dump");
    std::lock_guard lock(mutex);
    std::vector<uint8_t> buffer(gpuCrashDumpSize);
    std::copy_n(static_cast<const uint8_t*>(gpuCrashDump), gpuCrashDumpSize, buffer.begin());
    if(!TRAP::FileSystem::FileOrFolderExists(folderPath))
        TRAP::FileSystem::CreateFolder(folderPath);
    TRAP::FileSystem::WriteFile(filePath, buffer);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void LoadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
#ifdef TRAP_PLATFORM_WINDOWS
    handle = TRAP::Utils::DynamicLoading::LoadLibrary("GFSDK_Aftermath_Lib.x64.dll");
#elif defined(TRAP_PLATFORM_LINUX)
    handle = TRAP::Utils::DynamicLoading::LoadLibrary("libGFSDK_Aftermath_Lib.x64.so");
#endif

    enableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_EnableGpuCrashDumps>
    (
        handle, "GFSDK_Aftermath_EnableGpuCrashDumps"
    );
    disableGPUCrashDumps = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_DisableGpuCrashDumps>
    (
        handle, "GFSDK_Aftermath_DisableGpuCrashDumps"
    );
    getGPUCrashDumpStatus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_GetCrashDumpStatus>
    (
        handle, "GFSDK_Aftermath_GetCrashDumpStatus"
    );

    //DirectX 12 stuff
#if defined(__d3d11_h__) || defined(__d3d12_h__)
    dx12Initialize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_DX12_Initialize>
    (
        handle, "GFSDK_Aftermath_DX12_Initialize"
    );
    dx12CreateContextHandle = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_DX12_CreateContextHandle>
    (
        handle, "GFSDK_Aftermath_DX12_CreateContextHandle"
    );
    releaseContextHandle = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_ReleaseContextHandle>
    (
        handle, "GFSDK_Aftermath_ReleaseContextHandle"
    );
    setEventMarker = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GFSDK_Aftermath_SetEventMarker>
    (
        handle, "GFSDK_Aftermath_SetEventMarker"
    );
#endif

#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void UnloadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    TRAP::Utils::DynamicLoading::FreeLibrary(handle);

    handle = nullptr;
    enableGPUCrashDumps = nullptr;
    disableGPUCrashDumps = nullptr;
    getGPUCrashDumpStatus = nullptr;

    //DirectX 12 stuff
#if defined(__d3d11_h__) || defined(__d3d12_h__)
    dx12Initialize = nullptr;
    dx12CreateContextHandle = nullptr;
    releaseContextHandle = nullptr;
    setEventMarker = nullptr;
#endif

#endif
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[maybe_unused]] bool TRAP::Graphics::AftermathTracker::Initialize()
{
    if(initialized)
        return true;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Creating AftermathTracker");
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
    LoadFunctions();

    /*if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::D3D12)
    {
        GFSDK_Aftermath_Result res = dx12Initialize(GFSDK_Aftermath_Version_API,
                                                    GFSDK_Aftermath_FeatureFlags_Minimum |
                                                    GFSDK_Aftermath_FeatureFlags_EnableMarkers,
                                                    d3d12Device);
        if(res == GFSDK_Aftermath_Result_Success)
            enabled = true;
        else
            TP_WARN(Log::RendererAftermathTrackerPrefix, "Failed to initialize AftermathTracker");

        res = dx12CreateContextHandle(d3d12Device, &aftermathHandle);
        if(res == GFSDK_Aftermath_Result_Success)
            initialized = true;
        else
            TP_WARN(Log::RendererAftermathTrackerPrefix, "Failed to create AftermathTracker context");
    }
    else*/ if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
        initialized = true;

    AftermathCall(enableGPUCrashDumps(GFSDK_Aftermath_Version_API,
                                      GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan,
                                      GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default,
                                      OnCrashDump, nullptr, nullptr, nullptr));
#endif

    return initialized;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::Shutdown()
{
    if(!initialized)
        return;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
    AftermathCall(disableGPUCrashDumps());

    /*if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::D3D12 && context)
    {
        AftermathCall(releaseContextHandle(aftermathHandle));
    }*/

    UnloadFunctions();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::SetAftermathMarker([[maybe_unused]] const std::string_view name)
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    /*if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::D3D12 && context)
    {
        AftermathCall(setEventMarker(aftermathHandle, name.data(), name.size()));
    }*/
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
void TRAP::Graphics::AftermathTracker::AftermathCall(const GFSDK_Aftermath_Result res)
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

//-------------------------------------------------------------------------------------------------------------------//

GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status* outStatus)
{
    if(!initialized)
        return GFSDK_Aftermath_Result_FAIL_NotInitialized;

    return getGPUCrashDumpStatus(outStatus);
}
#endif
