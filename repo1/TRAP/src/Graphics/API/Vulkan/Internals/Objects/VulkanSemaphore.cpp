#include "TRAPPCH.h"
#include "VulkanSemaphore.h"

#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"

TRAP::Graphics::API::Vulkan::Semaphore::Semaphore(Device* device)
	: m_semaphore(), m_device(device)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = Initializers::SemaphoreCreateInfo();
	vkCreateSemaphore(m_device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_semaphore);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Semaphore::~Semaphore()
{
	if (m_semaphore)
	{
		vkDestroySemaphore(m_device->GetDevice(), m_semaphore, nullptr);
		m_semaphore = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkSemaphore& TRAP::Graphics::API::Vulkan::Semaphore::GetSemaphore()
{
	return m_semaphore;
}