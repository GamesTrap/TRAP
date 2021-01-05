#ifndef _TRAP_VULKANSEMAPHORE_H_
#define _TRAP_VULKANSEMAPHORE_H_

#include "VulkanSwapChain.h"
#include "Graphics/API/Objects/Semaphore.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanSemaphore final : public Semaphore
	{
	public:
		explicit VulkanSemaphore();
		~VulkanSemaphore();

		VkSemaphore& GetVkSemaphore();
		
	private:
		friend uint32_t TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore, const TRAP::Ref<Fence>& fence) const;
		
		VkSemaphore m_semaphore;
		
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANSEMAPHORE_H_*/