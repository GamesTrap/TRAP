#include "TRAPPCH.h"
#include "VulkanCommandPool.h"

#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanCommandPool::VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc)
	: CommandPool(desc.Queue)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanCommandPool(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Creating CommandPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkCommandPoolCreateInfo info = VulkanInits::CommandPoolCreateInfo
		(
			std::dynamic_pointer_cast<VulkanQueue>(m_queue)->GetQueueFamilyIndex()
		);
	if ((desc.CreateFlags & RendererAPI::CommandPoolCreateFlags::Transient) != RendererAPI::CommandPoolCreateFlags::None)
		info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	if ((desc.CreateFlags & RendererAPI::CommandPoolCreateFlags::ResetCommandBuffer) != RendererAPI::CommandPoolCreateFlags::None)
		info.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCall(vkCreateCommandPool(m_device->GetVkDevice(), &info, nullptr, &m_vkCommandPool));
	TRAP_ASSERT(m_vkCommandPool, "VulkanCommandPool(): Vulkan CommandPool is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandPool::~VulkanCommandPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_commandBuffers.clear();

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Destroying CommandPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyCommandPool(m_device->GetVkDevice(), m_vkCommandPool, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::VulkanCommandPool::AllocateCommandBuffer(const bool secondary)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	return m_commandBuffers.emplace_back(TRAP::MakeScope<VulkanCommandBuffer>(m_device, m_queue, m_vkCommandPool, secondary)).get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::FreeCommandBuffer(const CommandBuffer* const cmdBuffer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(cmdBuffer, "VulkanCommandPool::FreeCommandBuffer(): CommandBuffer is nullptr");

	for(usize i = 0; i < m_commandBuffers.size(); i++)
	{
		if(m_commandBuffers[i].get() != cmdBuffer)
			continue;

		m_commandBuffers[i] = std::move(m_commandBuffers.back());
		m_commandBuffers.pop_back();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::Reset() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCall(vkResetCommandPool(m_device->GetVkDevice(), m_vkCommandPool, 0));
}