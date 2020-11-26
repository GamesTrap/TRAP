#include "TRAPPCH.h"
#include "VulkanSemaphore.h"

#include "VulkanInits.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanSemaphore::VulkanSemaphore(TRAP::Ref<VulkanDevice> device)
	: m_semaphore(VK_NULL_HANDLE), m_signaled(false), m_device(std::move(device))
{
	VkSemaphoreCreateInfo info = VulkanInits::SemaphoreCreateInfo();
	VkCall(vkCreateSemaphore(m_device->GetVkDevice(), &info, nullptr, &m_semaphore));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSemaphore::~VulkanSemaphore()
{
	if(m_semaphore)
	{
		vkDestroySemaphore(m_device->GetVkDevice(), m_semaphore, nullptr);
		m_semaphore = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphore& TRAP::Graphics::API::VulkanSemaphore::GetVkSemaphore()
{
	return m_semaphore;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanSemaphore::IsSignaled() const
{
	return m_signaled;
}
