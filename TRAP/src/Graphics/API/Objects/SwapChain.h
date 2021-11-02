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
		static TRAP::Ref<SwapChain> Create(RendererAPI::SwapChainDesc& desc);

		static TRAP::Graphics::API::ImageFormat GetRecommendedSwapchainFormat(bool HDR);

		virtual ~SwapChain();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		SwapChain(const SwapChain&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		SwapChain& operator=(const SwapChain&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		SwapChain(SwapChain&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		SwapChain& operator=(SwapChain&&) = default;

		virtual uint32_t AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                  const TRAP::Ref<Fence>& fence) const = 0;

		virtual const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const;

		virtual void ToggleVSync() = 0;

	protected:
		SwapChain();

		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargets;
	};
}

#endif /*TRAP_SWAPCHAIN_H*/