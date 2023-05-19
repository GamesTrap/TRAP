#include "TRAPPCH.h"
#include "ErrorCodes.h"

#include "Utils/Dialogs/Dialogs.h"
#include "Log/Log.h"

struct ErrorData
{
    std::string_view Title;
    std::string_view Description;
    std::string_view LogPrefix;
    std::string_view LogMessage;
};

static const std::unordered_map<TRAP::Utils::ErrorCode, ErrorData> s_errorMap
{
    {TRAP::Utils::ErrorCode::RenderAPIInvalid, {"Unsupported device", "Device is unsupported!\nNo compatible RenderAPI selected!\nError code: 0x0002", TRAP::Log::RendererPrefix, "No compatible RenderAPI found! (0x0002)"}},
    {TRAP::Utils::ErrorCode::VulkanSurfaceExtensionsUnsupported, {"Vulkan API error", "Mandatory Vulkan surface extensions are unsupported!\nError code: 0x0003", TRAP::Log::RendererVulkanPrefix, "Vulkan surface extensions are unsupported! (0x0003)"}},
    {TRAP::Utils::ErrorCode::VulkanSwapchainExtensionsUnsupported, {"Vulkan API error", "Mandatory Vulkan swapchain extension is unsupported!\nError code: 0x0004", TRAP::Log::RendererVulkanPrefix, "Vulkan swapchain extension is unsupported! (0x0004)"}},
    {TRAP::Utils::ErrorCode::VulkanInstanceCreationFailed, {"Vulkan API error", "Vulkan instance creation failed!\nError code: 0x0005", TRAP::Log::RendererVulkanInstancePrefix, "Vulkan instance creation failed! (0x0005)"}},
    {TRAP::Utils::ErrorCode::VulkanPhysicalDeviceCreationFailed, {"Vulkan API error", "Vulkan physical device creation failed!\nError code: 0x0006", TRAP::Log::RendererVulkanPhysicalDevicePrefix, "Vulkan physical device creation failed! (0x0006)"}},
    {TRAP::Utils::ErrorCode::VulkanNoPhysicalDeviceFound, {"Vulkan API error", "No Vulkan capable physical device found!\nError code: 0x0007", TRAP::Log::RendererVulkanPrefix, "No Vulkan capable physical device found! (0x0007)"}},
    {TRAP::Utils::ErrorCode::LinuxUnsupportedWindowManager, {"No supported window manager", "No supported window manager found!\nTRAP™ only supports X11 and Wayland\nMake sure the appropriate environment variable(s) is/are set!\nError code: 0x0008", TRAP::Log::EngineLinuxPrefix, "No supported window manager found! (0x0008)"}},
    {TRAP::Utils::ErrorCode::WindowingAPIWindowCreationFailed, {"Failed to create window", "Failed to create window!\nError code: 0x0009", TRAP::Log::WindowPrefix, "Failed to create window! (0x0009)"}},
    {TRAP::Utils::ErrorCode::WindowsVersionOlderThanWindows7, {"Unsupported Windows version", "Unsupported Windows version!\nTRAP™ needs Windows 7 or newer\nError code: 0x000A", TRAP::Log::EngineWindowsPrefix, "Windows version is older than Windows 7! (0x000A)"}},
    {TRAP::Utils::ErrorCode::RenderAPINoSupportedFound, {"Incompatible device (GPU)", "TRAP™ was unable to detect a compatible RenderAPI!\nDoes your system meet the minimum system requirements for running TRAP™?\nPlease check your GPU driver!\nError code: 0x000B", TRAP::Log::RendererPrefix, "No supported RenderAPI found! (0x000B)"}},
    {TRAP::Utils::ErrorCode::MonitorNoneFound, {"No monitor found!", "No monitor found!\nError code: 0x000C", TRAP::Log::WindowPrefix, "No monitor found! (0x000C)"}},
    {TRAP::Utils::ErrorCode::VulkanDeviceReset, {"Vulkan API error", "Vulkan device was reset while presenting!\nError code: 0x000D", TRAP::Log::RendererVulkanPrefix, "Vulkan device reset while presenting! (0x000D)"}},
    {TRAP::Utils::ErrorCode::VulkanPresentationFailed, {"Vulkan API error", "Vulkan presenting failed!\nError code: 0x000F", TRAP::Log::RendererVulkanPrefix, "Vulkan presenting failed! (0x000F)"}},
    {TRAP::Utils::ErrorCode::VulkanSwapchainCreationFailed, {"Vulkan API error", "Failed to create new Vulkan SwapChain!\nError code: 0x0010", TRAP::Log::RendererVulkanPrefix, "Failed to create new Vulkan SwapChain! (0x0010)"}},
    {TRAP::Utils::ErrorCode::WindowingAPIFailedInitialization, {"WindowingAPI error", "Failed to initialize WindowingAPI!\nError code: 0x0011", TRAP::Log::WindowPrefix, "Failed to initialize WindowingAPI! (0x0011)"}},
    {TRAP::Utils::ErrorCode::ApplicationIsAlreadyRunning, {"TRAP™ is already running", "A TRAP™ application is already running!\nError code: 0x0012", TRAP::Log::ApplicationPrefix, "A TRAP™ application is already running! (0x0012)"}},
    {TRAP::Utils::ErrorCode::ImGuiFailedInitialization, {"ImGui error", "Failed to initialize ImGui!\nError code: 0x0013", TRAP::Log::ImGuiPrefix, "Failed to initialize ImGui! (0x0013)"}},
    {TRAP::Utils::ErrorCode::VulkanSwapchainImageAcquireFailed, {"Vulkan API error", "Vulkan failed to acquire next swapchain image!\nError code: 0x0014", TRAP::Log::RendererVulkanPrefix, "Vulkan failed to acquire next swapchain image! (0x0014)"}},
    {TRAP::Utils::ErrorCode::ApplicationNotLaunchedWithSteam, {"Application error", "Please launch the game through Steam!\nError code: 0x0015", TRAP::Log::SteamworksSDKPrefix, "Application not launched through steam! (0x0015)"}},
    {TRAP::Utils::ErrorCode::SteamNotRunning, {"Application error", "Steam must be running to play the game!\nError code: 0x0016", TRAP::Log::SteamworksSDKPrefix, "Steam must be running to play the game! (0x0016)"}},
    {TRAP::Utils::ErrorCode::VulkanDeviceLost, {"Vulkan API error", "Vulkan device was lost!\nError code: 0x000D", TRAP::Log::RendererVulkanPrefix, "Vulkan device lost! (0x000D)"}},
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::DisplayError(const ErrorCode error)
{
    const ErrorData& errorData = s_errorMap.at(error);

    Utils::Dialogs::ShowMsgBox(std::string(errorData.Title), std::string(errorData.Description),
                               Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);

    TP_CRITICAL(errorData.LogPrefix, errorData.LogMessage);

    TRAP_ASSERT(false, errorData.LogPrefix, errorData.LogMessage);

    exit(ToUnderlying(error));
}
