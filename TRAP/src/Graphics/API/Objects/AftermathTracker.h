#ifndef TRAP_AFTERMATHTRACKER_H
#define TRAP_AFTERMATHTRACKER_H

#include "Core/Base.h"

#ifdef ENABLE_NSIGHT_AFTERMATH
#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_Defines.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#endif /*ENABLE_NSIGHT_AFTERMATH*/

namespace TRAP::Graphics::AftermathTracker
{
	/// @brief Initialize NVIDIA Nsight Aftermath.
	/// @return True on successful initialization, false otherwise.
	[[maybe_unused]] bool Initialize();
	/// @brief Shutdown NVIDIA Nsight Aftermath.
	void Shutdown();

	/// @brief Set a marker.
	/// @param name Name of the marker.
	constexpr void SetAftermathMarker(std::string_view name) noexcept;

#ifdef ENABLE_NSIGHT_AFTERMATH
	/// @brief Check the given Aftermath result for errors.
	///        Logs a message in case of an error.
	/// @param res Aftermath result to check.
	constexpr void AftermathCall(GFSDK_Aftermath_Result res);

	/// @brief Retrieve the crash dump generation status.
	/// @param outStatus Output: Status of the crash dump generation.
	/// @return Result the crash dump status call.
	[[nodiscard]] GFSDK_Aftermath_Result GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus);
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::AftermathTracker::SetAftermathMarker([[maybe_unused]] const std::string_view name) noexcept
{
#ifdef ENABLE_NSIGHT_AFTERMATH
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
#endif /*ENABLE_NSIGHT_AFTERMATH*/

    //TODO Implement?!
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
constexpr void TRAP::Graphics::AftermathTracker::AftermathCall(const GFSDK_Aftermath_Result res)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

#endif /*TRAP_AFTERMATHTRACKER_H*/
