#include "TRAPPCH.h"
#include "VulkanCommandPool.h"

#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanCommandPool::VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_vkCommandPool(VK_NULL_HANDLE)
{
	m_queue = desc.Queue;

	TRAP_ASSERT(m_device, "device is nullptr");
	TRAP_ASSERT(m_queue, "queue is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Creating CommandPool");
#endif

	VkCommandPoolCreateInfo info = VulkanInits::CommandPoolCreateInfo
		(
			dynamic_cast<VulkanQueue*>(m_queue.get())->GetQueueFamilyIndex()
		);
	if (desc.Transient)
		info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	VkCall(vkCreateCommandPool(m_device->GetVkDevice(), &info, nullptr, &m_vkCommandPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandPool::~VulkanCommandPool()
{
	TRAP_ASSERT(m_vkCommandPool);

	for (auto& m_commandBuffer : m_commandBuffers)
		m_commandBuffer.reset();
	m_commandBuffers.clear();

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Destroying CommandPool");
#endif

	vkDestroyCommandPool(m_device->GetVkDevice(), m_vkCommandPool, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandPool TRAP::Graphics::API::VulkanCommandPool::GetVkCommandPool() const
{
	return m_vkCommandPool;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::VulkanCommandPool::AllocateCommandBuffer(const bool secondary)
{
	return m_commandBuffers.emplace_back(TRAP::MakeScope<VulkanCommandBuffer>(m_device, m_queue, m_vkCommandPool, secondary)).get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::FreeCommandBuffer(CommandBuffer* cmdBuffer)
{
	for(std::size_t i = 0; i < m_commandBuffers.size(); i++)
	{
		if(m_commandBuffers[i].get() != cmdBuffer)
			continue;

		TRAP::Scope<CommandBuffer> cmdBuf = std::move(m_commandBuffers[i]);
		cmdBuf.reset();

		m_commandBuffers[i] = std::move(m_commandBuffers.back());
		m_commandBuffers.pop_back();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::Reset() const
{
	VkCall(vkResetCommandPool(m_device->GetVkDevice(), m_vkCommandPool, 0));
}