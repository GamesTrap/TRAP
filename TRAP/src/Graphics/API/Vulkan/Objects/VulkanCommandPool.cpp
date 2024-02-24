#include "TRAPPCH.h"
#include "VulkanCommandPool.h"

#include "Utils/ErrorCodes/ErrorCodes.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanCommandPool::VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc)
	: CommandPool(desc.Queue)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	using enum TRAP::Graphics::RendererAPI::CommandPoolCreateFlags;

	TRAP_ASSERT(m_device, "VulkanCommandPool(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Creating CommandPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkCommandPoolCreateInfo info = VulkanInits::CommandPoolCreateInfo
		(
			std::dynamic_pointer_cast<VulkanQueue>(m_queue)->GetQueueFamilyIndex()
		);
	if ((desc.CreateFlags & Transient) != None)
		info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	if ((desc.CreateFlags & ResetCommandBuffer) != None)
		info.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCall(vkCreateCommandPool(m_device->GetVkDevice(), &info, nullptr, &m_vkCommandPool));
	TRAP_ASSERT(m_vkCommandPool, "VulkanCommandPool(): Vulkan CommandPool is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandPool::~VulkanCommandPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_commandBuffers.clear();

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandPoolPrefix, "Destroying CommandPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyCommandPool(m_device->GetVkDevice(), m_vkCommandPool, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::CommandBuffer& TRAP::Graphics::API::VulkanCommandPool::GetCommandBuffer(const bool secondary, const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto& cmdBuffer = m_commandBuffers.emplace_back(TRAP::MakeScope<VulkanCommandBuffer>(m_device, m_queue, m_vkCommandPool, secondary, name));
	if(!cmdBuffer)
	{
		TRAP_ASSERT(false);
		TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanFailedToAllocateCommandBuffer);
	}

	return *cmdBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::ReleaseCommandBuffer(const CommandBuffer& cmdBuffer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	std::erase_if(m_commandBuffers, [&cmdBuffer](const auto& cmd){return cmd.get() == &cmdBuffer;});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandPool::Reset() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	VkCall(vkResetCommandPool(m_device->GetVkDevice(), m_vkCommandPool, 0));
}
