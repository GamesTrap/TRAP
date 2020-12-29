#ifndef _TRAP_VULKANSWAPCHAIN_H_
#define _TRAP_VULKANSWAPCHAIN_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSurface;
	class VulkanRenderTarget;

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(const TRAP::Ref<VulkanInstance>& instance,
		                TRAP::Ref<VulkanDevice> device,
		                const TRAP::Ref<VulkanMemoryAllocator>& vma,
		                RendererAPI::SwapChainDesc& desc);
		~VulkanSwapChain();
		
	private:
		TRAP::Ref<VulkanDevice> m_device;
		
		//Render targets created from the swapchain back buffers
		std::vector<TRAP::Ref<VulkanRenderTarget>> m_renderTargets;

		//Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated present queue)
		VkQueue m_presentQueue;
		VkSwapchainKHR m_swapChain;
		TRAP::Ref<VulkanSurface> m_surface;
		//RendererAPI::SwapChainDesc*
		uint32_t m_presentQueueFamilyIndex;
		uint32_t m_imageCount;
		bool m_enableVSync;

		RendererAPI::SwapChainDesc m_desc;
	};
}

#endif /*_TRAP_VULKANSWAPCHAIN_H_*/