#include "TRAPPCH.h"
#include "VulkanSemaphore.h"

#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanSemaphore::VulkanSemaphore(const SemaphoreType semaphoreType, [[maybe_unused]] const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanSemaphore(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Creating Semaphore");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkSemaphoreCreateInfo info = VulkanInits::SemaphoreCreateInfo();

	VkSemaphoreTypeCreateInfoKHR typeInfo{};
	if(semaphoreType == SemaphoreType::Timeline)
	{
		typeInfo = VulkanInits::SemaphoreTypeCreateInfo(VK_SEMAPHORE_TYPE_TIMELINE_KHR);
		info.pNext = &typeInfo;
	}

	VkCall(vkCreateSemaphore(m_device->GetVkDevice(), &info, nullptr, &m_semaphore));
	TRAP_ASSERT(m_semaphore, "VulkanSemaphore(): Vulkan Semaphore is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	if(!name.empty())
		TRAP::Graphics::API::VkSetObjectName(*m_device, std::bit_cast<u64>(m_semaphore), VK_OBJECT_TYPE_SEMAPHORE, name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
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
