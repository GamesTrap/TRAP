#ifndef TRAP_VULKANSWAPCHAIN_H
#define TRAP_VULKANSWAPCHAIN_H

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
		~VulkanSwapChain() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSwapChain(const VulkanSwapChain&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSwapChain& operator=(const VulkanSwapChain&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSwapChain(VulkanSwapChain&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSwapChain& operator=(VulkanSwapChain&&) = default;

		uint32_t AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                          const TRAP::Ref<Fence>& fence) const override;

		void ToggleVSync() override;

		VkSwapchainKHR GetVkSwapChain() const;
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

#endif /*TRAP_VULKANSWAPCHAIN_H*/