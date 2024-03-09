#include "TRAPPCH.h"
#include "VulkanSemaphore.h"

#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanSemaphore::VulkanSemaphore()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanSemaphore(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Creating Semaphore");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const VkSemaphoreCreateInfo info = VulkanInits::SemaphoreCreateInfo();
	VkCall(vkCreateSemaphore(m_device->GetVkDevice(), &info, nullptr, &m_semaphore));
	TRAP_ASSERT(m_semaphore, "VulkanSemaphore(): Vulkan Semaphore is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSemaphore::~VulkanSemaphore()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Destroying Semaphore");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroySemaphore(m_device->GetVkDevice(), m_semaphore, nullptr);
}
