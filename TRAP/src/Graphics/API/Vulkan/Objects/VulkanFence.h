#ifndef _TRAP_VULKANFENCE_H_
#define _TRAP_VULKANFENCE_H_

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanFence
	{
	public:
		explicit VulkanFence(TRAP::Ref<VulkanDevice> device);
		~VulkanFence();

		VkFence& GetVkFence();

		bool IsSubmitted() const;
		
	private:
		VkFence m_fence;
		bool m_submitted;
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANFENCE_H_*/