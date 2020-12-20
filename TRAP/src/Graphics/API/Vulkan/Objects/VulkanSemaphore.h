#ifndef _TRAP_VULKANSEMAPHORE_H_
#define _TRAP_VULKANSEMAPHORE_H_

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
		
		VkSemaphore m_semaphore;
		bool m_signaled;
		
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANSEMAPHORE_H_*/