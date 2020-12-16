#ifndef _TRAP_VULKANCOMMANDBUFFER_H_
#define _TRAP_VULKANCOMMANDBUFFER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandPool;
	class VulkanDevice;
	class VulkanQueue;

	class VulkanCommandBuffer
	{
	public:
		~VulkanCommandBuffer();

		VkCommandBuffer& GetVkCommandBuffer();
		RendererAPI::QueueType GetQueueType() const;
		bool IsSecondary() const;

		void BindPushConstants(const TRAP::Ref<VulkanRootSignature>& rootSignature, const char* name, const void* constants) const;
		void BindPushConstantsByIndex(const TRAP::Ref<VulkanRootSignature>& rootSignature, uint32_t paramIndex, const void* constants) const;

		void AddDebugMarker(float r, float g, float b, const char* name) const;
		void BeginDebugMarker(float r, float g, float b, const char* name) const;
		void EndDebugMarker() const;
		
	private:
		friend VulkanCommandPool;
		
		VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanQueue> queue, VkCommandPool& commandPool, bool secondary);

		VkCommandBuffer m_vkCommandBuffer;

		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanQueue> m_queue;
		VkCommandPool& m_vkCommandPool;
		bool m_secondary;
		
		//VkRenderPass m_vkActiveRenderPass;
		//VkPipelineLayout m_boundPipelineLayout;
	};
}

#endif /*_TRAP_VULKANCOMMANDBUFFER_H_*/