#ifndef TRAP_AFTERMATHTRACKER_H
#define TRAP_AFTERMATHTRACKER_H

#include <string>

namespace TRAP::Graphics::AftermathTracker
{
	/// <summary>
	/// Initialize NVIDIA Nsight Aftermath.
	/// </summary>
	/// <returns>True on successful initialization, false otherwise.</returns>
	[[maybe_unused]] bool Initialize();
	/// <summary>
	/// Shutdown NVIDIA Nsight Aftermath.
	/// </summary>
	void Shutdown();

	/// <summary>
	/// Set a marker.
	/// </summary>
	/// <param name="name">Name of the marker.</param>
	void SetAftermathMarker(std::string_view name) noexcept;

#ifdef ENABLE_NSIGHT_AFTERMATH
	/// <summary>
	/// Check the given Aftermath result for errors.
	///
	/// Logs a message in case of an error.
	/// </summary>
	/// <param name="res">Aftermath result to check.</param>
	void AftermathCall(GFSDK_Aftermath_Result res);

	/// <summary>
	/// Retrieve the crash dump generation status.
	/// </summary>
	/// <param name="outStatus">Output: Status of the crash dump generation.</param>
	/// <returns>Result the crash dump status call.</returns>
	[[nodiscard]] GFSDK_Aftermath_Result GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus);
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

#endif /*TRAP_AFTERMATHTRACKER_H*/