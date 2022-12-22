#include "TRAPPCH.h"
#include "VulkanSemaphore.h"

#include "VulkanInits.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanSemaphore::VulkanSemaphore()
	: m_semaphore(VK_NULL_HANDLE),
	  m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice())
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanSemaphore(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Creating Semaphore");
#endif

	VkSemaphoreCreateInfo info = VulkanInits::SemaphoreCreateInfo();
	VkCall(vkCreateSemaphore(m_device->GetVkDevice(), &info, nullptr, &m_semaphore));
	TRAP_ASSERT(m_semaphore, "VulkanSemaphore(): Vulkan Semaphore is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSemaphore::~VulkanSemaphore()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Destroying Semaphore");
#endif

	vkDestroySemaphore(m_device->GetVkDevice(), m_semaphore, nullptr);
	m_semaphore = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkSemaphore TRAP::Graphics::API::VulkanSemaphore::GetVkSemaphore() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_semaphore;
}
