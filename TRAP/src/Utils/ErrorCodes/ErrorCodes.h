#ifndef TRAP_ERRORCODES_H
#define TRAP_ERRORCODES_H

#include "Core/Types.h"

namespace TRAP::Utils
{
    enum class ErrorCode : i32
    {
        // LinuxUsingWayland = 0x0001, //REMOVED
        RenderAPIInvalid = 0x0002,
        VulkanSurfaceExtensionsUnsupported = 0x0003,
        VulkanSwapchainExtensionsUnsupported = 0x0004,
        VulkanInstanceCreationFailed = 0x0005,
        VulkanPhysicalDeviceCreationFailed = 0x0006,
        VulkanNoPhysicalDeviceFound = 0x0007,
        LinuxUnsupportedWindowManager = 0x0008,
        WindowingAPIWindowCreationFailed = 0x0009,
        // WindowsVersionOlderThanWindows7 = 0x000A, //Replaced by WindowsVersionOlderThanWindows10
        RenderAPINoSupportedFound = 0x000B,
        MonitorNoneFound = 0x000C,
        VulkanDeviceReset = 0x000D, //Usually happens while presenting (Nsight Aftermath may create a dump)
        //0x000E, //REMOVED
        VulkanPresentationFailed = 0x000F,
        VulkanSwapchainCreationFailed = 0x0010,
        WindowingAPIFailedInitialization = 0x0011,
        ApplicationIsAlreadyRunning = 0x0012,
        ImGuiFailedInitialization = 0x0013,
        VulkanSwapchainImageAcquireFailed = 0x0014,
        ApplicationNotLaunchedWithSteam = 0x0015,
        SteamNotRunning = 0x0016,
        VulkanDeviceLost = 0x0017,
        VulkanNoMatchingMemoryTypeFound = 0x0018,
        SteamFailedGeneric = 0x0019,
        SteamVersionMismatch = 0x001A,
        WindowsVersionOlderThanWindows10 = 0x001B,
        VulkanFailedToAllocateCommandBuffer = 0x001C,
        VulkanNoMatchingSurfaceFormatFound = 0x001D,
        VulkanNoQueueWithPresentationSupportFound = 0x001F,
        VulkanNoSupportedCompositeAlphaFlagFound = 0x0020
    };

//-------------------------------------------------------------------------------------------------------------------//

    /// @brief Display the error message from the given error code.
    /// Afterwards the engine will be closed.
    /// @param error Error code.
    [[noreturn]] void DisplayError(ErrorCode error);
}

#endif /*TRAP_ERRORCODES_H*/