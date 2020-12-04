#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanQueue.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	if(m_vkCommandBuffer)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif
		
		vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device,
                                                              TRAP::Ref<VulkanQueue> queue,
                                                              VkCommandPool& commandPool,
                                                              const bool secondary)
	: m_vkCommandBuffer(VK_NULL_HANDLE),
	  m_device(std::move(device)),
	  m_queue(std::move(queue)),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary)
{
	TRAP_ASSERT(device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Creating CommandBuffer");
#endif

	VkCommandBufferAllocateInfo info = VulkanInits::CommandBufferAllocateInfo(commandPool, secondary);

	VkCall(vkAllocateCommandBuffers(m_device->GetVkDevice(), &info, &m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBuffer& TRAP::Graphics::API::VulkanCommandBuffer::GetVkCommandBuffer()
{
	return m_vkCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanCommandBuffer::GetQueueType() const
{
	return m_queue->GetQueueType();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanCommandBuffer::IsSecondary() const
{
	return m_secondary;
}