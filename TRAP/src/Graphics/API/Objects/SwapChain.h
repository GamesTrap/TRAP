#ifndef _TRAP_SWAPCHAIN_H_
#define _TRAP_SWAPCHAIN_H_

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

		virtual uint32_t AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                  const TRAP::Ref<Fence>& fence) const = 0;

		virtual const std::vector<TRAP::Ref<RenderTarget>>& GetRenderTargets() const;

		virtual void ToggleVSync() = 0;

	protected:
		SwapChain() = default;

		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<RenderTarget>> m_renderTargets;
	};
}

#endif /*_TRAP_SWAPCHAIN_H_*/