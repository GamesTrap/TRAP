#ifndef _TRAP_VULKANSWAPCHAIN_H_
#define _TRAP_VULKANSWAPCHAIN_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/SwapChain.h"


namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSurface;

	class VulkanSwapChain final : public SwapChain
	{
	public:
		explicit VulkanSwapChain(RendererAPI::SwapChainDesc& desc);
		~VulkanSwapChain();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VulkanSwapChain(const VulkanSwapChain&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanSwapChain& operator=(const VulkanSwapChain&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanSwapChain(VulkanSwapChain&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanSwapChain& operator=(VulkanSwapChain&&) = default;

		uint32_t AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                          const TRAP::Ref<Fence>& fence) const override;

		void ToggleVSync() override;

		const VkSwapchainKHR& GetVkSwapChain() const;
		VkQueue GetPresentVkQueue() const;

	private:
		void AddSwapchain(RendererAPI::SwapChainDesc& desc);
		void RemoveSwapchain();

		TRAP::Ref<VulkanMemoryAllocator> m_vma;
		TRAP::Ref<VulkanInstance> m_instance;
		TRAP::Ref<VulkanDevice> m_device;

		//Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated
		//present queue)
		VkQueue m_presentQueue;
		VkSwapchainKHR m_swapChain;
		TRAP::Ref<VulkanSurface> m_surface;
		uint32_t m_presentQueueFamilyIndex;
		uint32_t m_imageCount;
		bool m_enableVSync;

		RendererAPI::SwapChainDesc m_desc;
	};
}

#endif /*_TRAP_VULKANSWAPCHAIN_H_*/