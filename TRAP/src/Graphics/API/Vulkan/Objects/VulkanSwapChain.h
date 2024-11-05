#ifndef TRAP_VULKANSWAPCHAIN_H
#define TRAP_VULKANSWAPCHAIN_H

#ifndef TRAP_HEADLESS_MODE

#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/SwapChain.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSurface;

	/// @remark @headless This class is not available in headless mode.
	class VulkanSwapChain final : public SwapChain
	{
	public:
		/// @brief Constructor.
		/// @param desc Swap chain description.
		explicit VulkanSwapChain(RendererAPI::SwapChainDesc& desc);
		/// @brief Destructor.
		~VulkanSwapChain() override;

		/// @brief Copy constructor.
		consteval VulkanSwapChain(const VulkanSwapChain&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanSwapChain& operator=(const VulkanSwapChain&) noexcept = delete;
		/// @brief Move constructor.
		VulkanSwapChain(VulkanSwapChain&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanSwapChain& operator=(VulkanSwapChain&&) noexcept = default;

		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] std::optional<u32> AcquireNextImage(Semaphore& signalSemaphore,
		                                                  Fence& fence) const override;
		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] std::optional<u32> AcquireNextImage(Semaphore& signalSemaphore) const override;
		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] std::optional<u32> AcquireNextImage(Fence& fence) const override;

		/// @brief Toggle VSync on and off.
		void ToggleVSync() override;

		/// @brief Updates the framebuffer size and recreates the swap chain.
		void UpdateFramebufferSize() override;

		/// @brief Retrieve latency reports from NVIDIA-Reflex.
		/// @param numLatencyData Number of latency data points to return. Set to 0 to retrieve the maximum queryable frame data.
		/// @return Latency reports.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] std::vector<VkLatencyTimingsFrameReportNV> ReflexGetLatency(u32 numLatencyData) const override;
		/// @brief Set a timestamp for NVIDIA-Reflex.
		/// @param marker Enum value of the marker to set.
		void ReflexSetMarker(TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMarker marker) override;
		/// @brief Set the latency mode for NVIDIA-Reflex.
		/// @param latencyMode Latency mode to use.
		/// @param fpsLimit Optional: FPS limit, use 0 to disable limiter.
		void ReflexSetLatencyMode(TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode latencyMode, u32 fpsLimit = 0u) const override;
		/// @brief Delay host CPU work for low latency rendering.
		/// @param reflexSemaphore Semaphore to use for sleep.
		void ReflexSleep(const Semaphore& reflexSemaphore) const override;

		/// @brief Set a timestamp for AMD Anti Lag.
		/// @param marker Enum value of the marker to set.
		/// @param viewportData The viewport to set the marker for.
		void AntiLagSetMarker(TRAP::Graphics::RendererAPI::AMDAntiLagMarker marker, const RendererAPI::PerViewportData& viewportData) override;
		/// @brief Set the mode for AMD Anti Lag.
		/// @param mode Mode to use.
		/// @param fpsLimit Optional: FPS limit, use 0 to disable limiter.
		void AntiLagSetMode(TRAP::Graphics::RendererAPI::AMDAntiLagMode mode, u32 fpsLimit = 0u) const override;

		/// @brief Retrieve the Vulkan swap chain handle.
		/// @return Vulkan swap chain handle.
		[[nodiscard]] constexpr VkSwapchainKHR GetVkSwapChain() const noexcept;
		/// @brief Retrieve the Vulkan queue used for presentation.
		/// @return Vulkan queue used for presentation.
		[[nodiscard]] constexpr VkQueue GetPresentVkQueue() const noexcept;

		/// @brief Retrieve the current present count of the swapchain. Used for PresentID and NVIDIA Reflex.
		/// @return Current present count.
		[[nodiscard]] constexpr u64 GetPresentCount() const noexcept;

	private:
		/// @brief Initialize the swap chain.
		/// @param desc Swap chain description.
		/// @param oldSwapChain Optional: Old Vulkan swapchain, potentially speeds up creation of new swapchain.
		void InitSwapchain(RendererAPI::SwapChainDesc& desc, VkSwapchainKHR oldSwapChain = nullptr);
		/// @brief Uninitialize the swap chain.
		void DeInitSwapchain();
		/// @brief Reinitialize the swap chain while reusing the old one.
		void ReInitSwapChain();

		TRAP::Ref<VulkanMemoryAllocator> m_vma = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetVMA();
		TRAP::Ref<VulkanInstance> m_instance = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetInstance();
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		//Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated
		//present queue)
		VkQueue m_presentQueue = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		TRAP::Ref<VulkanSurface> m_surface = nullptr;

		u64 m_presentCounter = 0u;

		RendererAPI::SwapChainDesc m_desc{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSwapchainKHR TRAP::Graphics::API::VulkanSwapChain::GetVkSwapChain() const noexcept
{
	return m_swapChain;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueue TRAP::Graphics::API::VulkanSwapChain::GetPresentVkQueue() const noexcept
{
	return m_presentQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::Graphics::API::VulkanSwapChain::GetPresentCount() const noexcept
{
	return m_presentCounter;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_VULKANSWAPCHAIN_H*/
