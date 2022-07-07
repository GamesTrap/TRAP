#ifndef TRAP_SWAPCHAIN_H
#define TRAP_SWAPCHAIN_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Semaphore;
	class Fence;

	class SwapChain
	{
	public:
		/// <summary>
		/// Create a new swap chain from the given description.
		/// </summary>
		/// <param name="desc">Swap chain description.</param>
		/// <returns>Created swap chain.</returns>
		static TRAP::Ref<SwapChain> Create(RendererAPI::SwapChainDesc& desc);

		/// <summary>
		/// Retrieve the recommended swap chain image format.
		/// </summary>
		/// <param name="HDR">Use HDR?</param>
		/// <param name="SRGB">Use SRGB?</param>
		/// <returns>Recommended swap chain image format.</returns>
		static TRAP::Graphics::API::ImageFormat GetRecommendedSwapchainFormat(bool HDR, bool SRGB);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~SwapChain();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		SwapChain(const SwapChain&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		SwapChain& operator=(const SwapChain&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		SwapChain(SwapChain&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		SwapChain& operator=(SwapChain&&) = default;

		/// <summary>
		/// Acquire the next presentable image from the swapchain to render to.
		/// </summary>
		/// <param name="signalSemaphore">Semaphore to signal when the image is ready to be presented.</param>
		/// <param name="fence">Fence to wait for the image to be ready to be presented.</param>
		/// <returns>Acuired image index.</returns>
		virtual uint32_t AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                  const TRAP::Ref<Fence>& fence) const = 0;

		/// <summary>
		/// Retrieve the render targets used by the swapchain.
		/// </summary>
		/// <returns>Render targets used by the swapchain.</returns>
		virtual const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const;
		/// <summary>
		/// Retrieve the render targets used by the swapchain to resolve MSAA.
		/// </summary>
		/// <returns>Render targets used by the swapchain to resolve MSAA.</returns>
		virtual const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargetsMSAA() const;

		/// <summary>
		/// Toggle Vsync on and off.
		/// </summary>
		virtual void ToggleVSync() = 0;

		/// <summary>
		/// Set sample count used by anti aliasing.
		/// </summary>
		virtual void SetSampleCount(RendererAPI::SampleCount sampleCount) = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		SwapChain();

		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargets;
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargetsMSAA;
	};
}

#endif /*TRAP_SWAPCHAIN_H*/