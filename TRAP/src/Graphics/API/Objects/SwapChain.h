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
		constexpr SwapChain(const SwapChain&) noexcept = default;
		/// @brief Copy assignment operator.
		SwapChain& operator=(const SwapChain&) noexcept = default;
		/// @brief Move constructor.
		constexpr SwapChain(SwapChain&&) noexcept = default;
		/// @brief Move assignment operator.
		SwapChain& operator=(SwapChain&&) noexcept = default;

		/// @brief Acquire the next presentable image from the swapchain to render to.
		/// @param signalSemaphore Semaphore to signal when the image is ready to be presented.
		/// @param fence Fence to wait for the image to be ready to be presented.
		/// @return Acuired image index.
		[[nodiscard]] virtual std::optional<u32> AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                               const TRAP::Ref<Fence>& fence) const = 0;

		/// @brief Retrieve the render targets used by the swapchain.
		/// @return Render targets used by the swapchain.
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const noexcept;

		/// @brief Toggle Vsync on and off.
		virtual void ToggleVSync() = 0;

		/// @brief Updates the framebuffer size and recreates the swap chain.
		virtual void UpdateFramebufferSize() = 0;

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