#include "TRAPPCH.h"
#include "VulkanCommandPool.h"

#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanCommandPool::VulkanCommandPool(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanQueue> queue, const bool transient)
	: m_device(std::move(device)), m_queue(std::move(queue)), m_vkCommandPool(VK_NULL_HANDLE)
{
	TRAP_ASSERT(m_device, "device is nullptr");
	TRAP_ASSERT(m_queue, "queue is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Creating CommandPool");
#endif
	
	VkCommandPoolCreateInfo info = VulkanInits::CommandPoolCreateInfo(m_queue->GetQueueFamilyIndex());
	if (transient)
		info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	VkCall(vkCreateCommandPool(m_device->GetVkDevice(), &info, nullptr, &m_vkCommandPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandPool::~VulkanCommandPool()
{
	if(m_vkCommandPool)
	{
		for (auto& m_commandBuffer : m_commandBuffers)
			m_commandBuffer.reset();
		m_commandBuffers.clear();
		
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Destroying CommandPool");
#endif
		
		vkDestroyCommandPool(m_device->GetVkDevice(), m_vkCommandPool, nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandPool& TRAP::Graphics::API::VulkanCommandPool::GetVkCommandPool()
{
	return m_vkCommandPool;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer* TRAP::Graphics::API::VulkanCommandPool::AllocateCommandBuffer(const bool secondary)
{	
	m_commandBuffers.push_back(TRAP::Scope<VulkanCommandBuffer>(new VulkanCommandBuffer(m_device, m_queue, m_vkCommandPool, secondary)));
	return m_commandBuffers.back().get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::FreeCommandBuffer(VulkanCommandBuffer* cmdBuffer)
{
	for(uint32_t i = 0; i < m_commandBuffers.size(); i++)
	{
		if(m_commandBuffers[i].get() == cmdBuffer)
		{
			TRAP::Scope<VulkanCommandBuffer> cmdBuf = std::move(m_commandBuffers[i]);
			cmdBuf.reset();

			m_commandBuffers[i] = std::move(m_commandBuffers.back());
			m_commandBuffers.pop_back();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::Reset()
{
	VkCall(vkResetCommandPool(m_device->GetVkDevice(), m_vkCommandPool, 0));
	m_commandBuffers.clear();
}