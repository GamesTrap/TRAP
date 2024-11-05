#ifndef TRAP_SWAPCHAIN_H
#define TRAP_SWAPCHAIN_H

#include <optional>

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	/// @brief Retrieve the recommended swap chain image format.
	/// @param HDR Use HDR?
	/// @param SRGB Use SRGB?
	/// @return Recommended swap chain image format.
	[[nodiscard]] TRAP::Graphics::API::ImageFormat GetRecommendedSwapchainFormat(bool HDR, bool SRGB) noexcept;

#ifndef TRAP_HEADLESS_MODE

	class Semaphore;
	class Fence;

	class SwapChain
	{
	public:
		/// @brief Create a new swap chain from the given description.
		/// @param desc Swap chain description.
		/// @return Created swap chain.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static TRAP::Ref<SwapChain> Create(RendererAPI::SwapChainDesc& desc);

		/// @brief Destructor.
		virtual ~SwapChain();

		/// @brief Copy constructor.
		consteval SwapChain(const SwapChain&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval SwapChain& operator=(const SwapChain&) noexcept = delete;
		/// @brief Move constructor.
		constexpr SwapChain(SwapChain&&) noexcept = default;
		/// @brief Move assignment operator.
		SwapChain& operator=(SwapChain&&) noexcept = default;

		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] virtual std::optional<u32> AcquireNextImage(Semaphore& signalSemaphore,
		                                                          Fence& fence) const = 0;
		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] virtual std::optional<u32> AcquireNextImage(Semaphore& signalSemaphore) const = 0;
		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acquired image index.
		[[nodiscard]] virtual std::optional<u32> AcquireNextImage(Fence& fence) const = 0;

		/// @brief Retrieve the render targets used by the swapchain.
		/// @return Render targets used by the swapchain.
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const noexcept;

		/// @brief Toggle Vsync on and off.
		virtual void ToggleVSync() = 0;

		/// @brief Updates the framebuffer size and recreates the swap chain.
		virtual void UpdateFramebufferSize() = 0;

		/// @brief Retrieve latency reports from NVIDIA-Reflex.
		/// @param numLatencyData Number of latency data points to return. Set to 0 to retrieve the maximum queryable frame data.
		/// @return Latency reports.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual std::vector<VkLatencyTimingsFrameReportNV> ReflexGetLatency(u32 numLatencyData) const = 0;
		/// @brief Set a timestamp for NVIDIA-Reflex.
		/// @param marker Enum value of the marker to set.
		virtual void ReflexSetMarker(TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMarker marker) = 0;
		/// @brief Set the latency mode for NVIDIA-Reflex.
		/// @param latencyMode Latency mode to use.
		/// @param fpsLimit Optional: FPS limit, use 0 to disable limiter.
		virtual void ReflexSetLatencyMode(TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode latencyMode, u32 fpsLimit = 0u) const = 0;
		/// @brief Delay host CPU work for low latency rendering.
		/// @param reflexSemaphore Semaphore to use for sleep.
		virtual void ReflexSleep(const Semaphore& reflexSemaphore) const = 0;

		/// @brief Set a timestamp for AMD Anti Lag.
		/// @param marker Enum value of the marker to set.
		/// @param viewportData The viewport to set the marker for.
		virtual void AntiLagSetMarker(TRAP::Graphics::RendererAPI::AMDAntiLagMarker marker, const RendererAPI::PerViewportData& viewportData) = 0;
		/// @brief Set the mode for AMD Anti Lag.
		/// @param mode Mode to use.
		/// @param fpsLimit Optional: FPS limit, use 0 to disable limiter.
		virtual void AntiLagSetMode(TRAP::Graphics::RendererAPI::AMDAntiLagMode mode, u32 fpsLimit = 0u) const = 0;

	protected:
		/// @brief Constructor.
		SwapChain();

		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargets{};
	};
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const noexcept
{
	return m_renderTargets;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_SWAPCHAIN_H*/
