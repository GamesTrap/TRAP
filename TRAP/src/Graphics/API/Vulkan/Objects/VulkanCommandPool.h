#ifndef _TRAP_VULKANCOMMANDPOOL_H_
#define _TRAP_VULKANCOMMANDPOOL_H_

namespace TRAP::Graphics::API
{
	class VulkanCommandBuffer;
	class VulkanQueue;
	class VulkanDevice;

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanQueue> queue, bool transient);
		~VulkanCommandPool();

		VkCommandPool& GetVkCommandPool();

		VulkanCommandBuffer* AllocateCommandBuffer(bool secondary);
		void FreeCommandBuffer(VulkanCommandBuffer* cmdBuffer);

		void Reset() const;
		
	private:		
		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanQueue> m_queue;
		
		VkCommandPool m_vkCommandPool;

		std::vector<TRAP::Scope<VulkanCommandBuffer>> m_commandBuffers;
	};
}

#endif /*_TRAP_VULKANCOMMANDPOOL_H_*/