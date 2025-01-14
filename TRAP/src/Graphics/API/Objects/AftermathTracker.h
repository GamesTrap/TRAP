#ifndef TRAP_AFTERMATHTRACKER_H
#define TRAP_AFTERMATHTRACKER_H

#include <string_view>

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
	/// @note In a constexpr context this function does nothing.
	constexpr void AftermathCall(GFSDK_Aftermath_Result res);

	/// @brief Retrieve the crash dump generation status.
	/// @param outStatus Output: Status of the crash dump generation.
	/// @return Result the crash dump status call.
	[[nodiscard]] GFSDK_Aftermath_Result GetGPUCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status& outStatus);

    namespace INTERNAL
    {
        /// @brief Check the given Aftermath result for errors.
        ///        Logs a message in case of an error.
        /// @param res Aftermath result to check.
        void AftermathCallImpl(GFSDK_Aftermath_Result res);
    }
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::AftermathTracker::SetAftermathMarker([[maybe_unused]] const std::string_view name) noexcept
{
#ifdef ENABLE_NSIGHT_AFTERMATH
#endif /*ENABLE_NSIGHT_AFTERMATH*/

    //TODO Implement?!
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
constexpr void TRAP::Graphics::AftermathTracker::AftermathCall(const GFSDK_Aftermath_Result res)
{
    if(!std::is_constant_evaluated())
    {
        INTERNAL::AftermathCallImpl(res);
    }
}
#endif /*ENABLE_NSIGHT_AFTERMATH*/

#endif /*TRAP_AFTERMATHTRACKER_H*/
