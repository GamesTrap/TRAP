#ifndef _TRAP_VULKANSEMAPHORE_H_
#define _TRAP_VULKANSEMAPHORE_H_

#include "VulkanSwapChain.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanSemaphore
	{
	public:
		explicit VulkanSemaphore(TRAP::Ref<VulkanDevice> device);
		~VulkanSemaphore();

		VkSemaphore& GetVkSemaphore();

		bool IsSignaled() const;
		
	private:
		friend VulkanQueue;
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<VulkanSemaphore>& signalSemaphore, const TRAP::Ref<VulkanFence>& fence) const;
		
		VkSemaphore m_semaphore;
		bool m_signaled;
		
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANSEMAPHORE_H_*/