#ifndef TRAP_SWAPCHAIN_H
#define TRAP_SWAPCHAIN_H

#include <optional>

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Semaphore;
	class Fence;

	class SwapChain
	{
	public:
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Create a new swap chain from the given description.
		/// </summary>
		/// <param name="desc">Swap chain description.</param>
		/// <returns>Created swap chain.</returns>
		[[nodiscard]] static TRAP::Ref<SwapChain> Create(RendererAPI::SwapChainDesc& desc);
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the recommended swap chain image format.
		/// </summary>
		/// <param name="HDR">Use HDR?</param>
		/// <param name="SRGB">Use SRGB?</param>
		/// <returns>Recommended swap chain image format.</returns>
		[[nodiscard]] static TRAP::Graphics::API::ImageFormat GetRecommendedSwapchainFormat(bool HDR, bool SRGB) noexcept;

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~SwapChain();

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Copy constructor.
		/// </summary>
		SwapChain(const SwapChain&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		SwapChain& operator=(const SwapChain&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		SwapChain(SwapChain&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		SwapChain& operator=(SwapChain&&) noexcept = default;

		/// <summary>
		/// Acquire the next presentable image from the swapchain to render to.
		/// </summary>
		/// <param name="signalSemaphore">Semaphore to signal when the image is ready to be presented.</param>
		/// <param name="fence">Fence to wait for the image to be ready to be presented.</param>
		/// <returns>Acuired image index.</returns>
		[[nodiscard]] virtual std::optional<uint32_t> AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                               const TRAP::Ref<Fence>& fence) const = 0;

		/// <summary>
		/// Retrieve the render targets used by the swapchain.
		/// </summary>
		/// <returns>Render targets used by the swapchain.</returns>
		[[nodiscard]] const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const noexcept;

		/// <summary>
		/// Toggle Vsync on and off.
		/// </summary>
		virtual void ToggleVSync() = 0;

		/// <summary>
		/// Updates the framebuffer size and recreates the swap chain.
		/// </summary>
		virtual void UpdateFramebufferSize() = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		SwapChain();

		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargets;
#endif /*TRAP_HEADLESS_MODE*/
	};
}

#endif /*TRAP_SWAPCHAIN_H*/