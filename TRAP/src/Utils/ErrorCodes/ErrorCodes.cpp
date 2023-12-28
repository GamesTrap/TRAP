#include "TRAPPCH.h"
#include "ErrorCodes.h"

#include "Utils/ConstexprMap.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Log/Log.h"

struct ErrorData
{
    std::string_view Title;
    std::string_view Description;
    std::string_view LogPrefix;
    std::string_view LogMessage;
};

static constexpr TRAP::Utils::ConstexprMap<TRAP::Utils::ErrorCode, ErrorData, 24> s_errorMap
{
    {
        {
            {TRAP::Utils::ErrorCode::VulkanSurfaceExtensionsUnsupported, {"Vulkan API error", "Mandatory Vulkan surface extensions are unsupported!\nError code: 0x0003", TRAP::Log::RendererVulkanPrefix, "Vulkan surface extensions are unsupported! (0x0003)"}},
            {TRAP::Utils::ErrorCode::VulkanSwapchainExtensionsUnsupported, {"Vulkan API error", "Mandatory Vulkan swapchain extension is unsupported!\nError code: 0x0004", TRAP::Log::RendererVulkanPrefix, "Vulkan swapchain extension is unsupported! (0x0004)"}},
            {TRAP::Utils::ErrorCode::VulkanInstanceCreationFailed, {"Vulkan API error", "Vulkan instance creation failed!\nError code: 0x0005", TRAP::Log::RendererVulkanInstancePrefix, "Vulkan instance creation failed! (0x0005)"}},
            {TRAP::Utils::ErrorCode::VulkanPhysicalDeviceCreationFailed, {"Vulkan API error", "Vulkan physical device creation failed!\nError code: 0x0006", TRAP::Log::RendererVulkanPhysicalDevicePrefix, "Vulkan physical device creation failed! (0x0006)"}},
            {TRAP::Utils::ErrorCode::VulkanNoPhysicalDeviceFound, {"Vulkan API error", "No Vulkan capable physical device found!\nError code: 0x0007", TRAP::Log::RendererVulkanPrefix, "No Vulkan capable physical device found! (0x0007)"}},
            {TRAP::Utils::ErrorCode::VulkanDeviceReset, {"Vulkan API error", "Vulkan device was reset while presenting!\nError code: 0x000D", TRAP::Log::RendererVulkanPrefix, "Vulkan device reset while presenting! (0x000D)"}},
            {TRAP::Utils::ErrorCode::VulkanPresentationFailed, {"Vulkan API error", "Vulkan presenting failed!\nError code: 0x000F", TRAP::Log::RendererVulkanPrefix, "Vulkan presenting failed! (0x000F)"}},
            {TRAP::Utils::ErrorCode::VulkanSwapchainCreationFailed, {"Vulkan API error", "Failed to create new Vulkan SwapChain!\nError code: 0x0010", TRAP::Log::RendererVulkanPrefix, "Failed to create new Vulkan SwapChain! (0x0010)"}},
            {TRAP::Utils::ErrorCode::VulkanSwapchainImageAcquireFailed, {"Vulkan API error", "Vulkan failed to acquire next swapchain image!\nError code: 0x0014", TRAP::Log::RendererVulkanPrefix, "Vulkan failed to acquire next swapchain image! (0x0014)"}},
            {TRAP::Utils::ErrorCode::VulkanDeviceLost, {"Vulkan API error", "Vulkan device was lost!\nError code: 0x0017", TRAP::Log::RendererVulkanPrefix, "Vulkan device lost! (0x0017)"}},
            {TRAP::Utils::ErrorCode::VulkanNoMatchingMemoryTypeFound, {"Vulkan API error", "Vulkan failed to find a matching memory type!\nError code: 0x0018", TRAP::Log::RendererVulkanPrefix, "Vulkan failed to find a matching memory type! (0x0018)"}},

            {TRAP::Utils::ErrorCode::ApplicationIsAlreadyRunning, {"TRAP™ is already running", "A TRAP™ application is already running!\nError code: 0x0012", TRAP::Log::ApplicationPrefix, "A TRAP™ application is already running! (0x0012)"}},
            {TRAP::Utils::ErrorCode::ImGuiFailedInitialization, {"ImGui error", "Failed to initialize ImGui!\nError code: 0x0013", TRAP::Log::ImGuiPrefix, "Failed to initialize ImGui! (0x0013)"}},
            {TRAP::Utils::ErrorCode::RenderAPIInvalid, ErrorData{"Unsupported device", "Device is unsupported!\nNo compatible RenderAPI selected!\nError code: 0x0002", TRAP::Log::RendererPrefix, "No compatible RenderAPI found! (0x0002)"}},
            {TRAP::Utils::ErrorCode::RenderAPINoSupportedFound, {"Incompatible device (GPU)", "TRAP™ was unable to detect a compatible RenderAPI!\nDoes your system meet the minimum system requirements for running TRAP™?\nPlease check your GPU driver!\nError code: 0x000B", TRAP::Log::RendererPrefix, "No supported RenderAPI found! (0x000B)"}},
            // {TRAP::Utils::ErrorCode::WindowsVersionOlderThanWindows7, {"Unsupported Windows version", "Unsupported Windows version!\nTRAP™ requires Microsoft Windows 7 or newer\nError code: 0x000A", TRAP::Log::EngineWindowsPrefix, "Windows version is older than Windows 7! (0x000A)"}},
            {TRAP::Utils::ErrorCode::WindowsVersionOlderThanWindows10, {"Unsupported operating system version", "Unsupported operating system version!\nTRAP™ requires Microsoft Windows 10 or newer\nError code: 0x001B", TRAP::Log::EngineWindowsPrefix, "Windows version is older than Windows 10! (0x001B)"}},
            {TRAP::Utils::ErrorCode::WindowingAPIFailedInitialization, {"WindowingAPI error", "Failed to initialize WindowingAPI!\nError code: 0x0011", TRAP::Log::WindowPrefix, "Failed to initialize WindowingAPI! (0x0011)"}},
            {TRAP::Utils::ErrorCode::WindowingAPIWindowCreationFailed, {"Failed to create window", "Failed to create window!\nError code: 0x0009", TRAP::Log::WindowPrefix, "Failed to create window! (0x0009)"}},
            {TRAP::Utils::ErrorCode::MonitorNoneFound, {"No monitor found!", "No monitor found!\nError code: 0x000C", TRAP::Log::WindowPrefix, "No monitor found! (0x000C)"}},
            {TRAP::Utils::ErrorCode::LinuxUnsupportedWindowManager, {"No supported window manager", "No supported window manager found!\nTRAP™ only supports X11 and Wayland\nMake sure the appropriate environment variable(s) is/are set!\nError code: 0x0008", TRAP::Log::EngineLinuxPrefix, "No supported window manager found! (0x0008)"}},

            {TRAP::Utils::ErrorCode::ApplicationNotLaunchedWithSteam, {"Application error", "Please launch the game through Steam!\nError code: 0x0015", TRAP::Log::SteamworksSDKPrefix, "Application not launched through steam! (0x0015)"}},
            {TRAP::Utils::ErrorCode::SteamNotRunning, {"Application error", "Steam must be running to play the game!\nError code: 0x0016", TRAP::Log::SteamworksSDKPrefix, "Steam must be running to play the game! (0x0016)"}},
            {TRAP::Utils::ErrorCode::SteamFailedGeneric, {"Application error", "Steam API failure!\nError code: 0x0019", TRAP::Log::SteamworksSDKPrefix, "Steam API failure! (0x0019)"}},
            {TRAP::Utils::ErrorCode::SteamVersionMismatch, {"Application error", "Steam client is out of date!\nPlease update Steam\nError code: 0x001A", TRAP::Log::SteamworksSDKPrefix, "Steam is out of date, please update the client! (0x001A)"}},
        }
    }
};

//-------------------------------------------------------------------------------------------------------------------//

#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /*_MSC_VER*/

[[noreturn]] void TRAP::Utils::DisplayError(const ErrorCode error)
{
    const auto errorData = s_errorMap.at(error);
    if(!errorData.has_value())
    {
        TRAP_ASSERT(false, Log::ApplicationPrefix, "Unknown error occurred!");

#if !defined(TRAP_HEADLESS_MODE) && !defined(TRAP_UNITTESTS)
        Utils::Dialogs::ShowMsgBox("Unknown error occurred!", "An unknown error has occurred!\nExiting TRAP Engine",
                                   Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
#endif /*!defined(TRAP_HEADLESS_MODE) && !defined(TRAP_UNITTESTS)*/

        TP_CRITICAL(Log::ApplicationPrefix, "Unknown error occurred!");

        throw std::runtime_error("Unknown error occurred!");
        std::terminate(); //Make sure we exit program even when using try/catch
    }

#if !defined(TRAP_HEADLESS_MODE) && !defined(TRAP_UNITTESTS)
    Utils::Dialogs::ShowMsgBox(std::string(errorData->Title), std::string(errorData->Description),
                               Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
#endif /*!defined(TRAP_HEADLESS_MODE) && !defined(TRAP_UNITTESTS)*/

    TP_CRITICAL(errorData->LogPrefix, errorData->LogMessage);

    TRAP_ASSERT(false, errorData->LogPrefix, errorData->LogMessage);

    throw std::runtime_error(errorData->LogMessage.data());
    std::terminate(); //Make sure we exit program even when using try/catch
}

#ifdef _MSC_VER
#pragma warning(default: 4702)
#endif /*_MSC_VER*/
