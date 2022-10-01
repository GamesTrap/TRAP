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
	ZoneScoped;

	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Creating Semaphore");
#endif

	VkSemaphoreCreateInfo info = VulkanInits::SemaphoreCreateInfo();
	VkCall(vkCreateSemaphore(m_device->GetVkDevice(), &info, nullptr, &m_semaphore));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSemaphore::~VulkanSemaphore()
{
	ZoneScoped;

	TRAP_ASSERT(m_semaphore);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSemaphorePrefix, "Destroying Semaphore");
#endif

	vkDestroySemaphore(m_device->GetVkDevice(), m_semaphore, nullptr);
	m_semaphore = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphore TRAP::Graphics::API::VulkanSemaphore::GetVkSemaphore() const
{
	ZoneScoped;

	return m_semaphore;
}
