#include "TRAPPCH.h"
#include "VulkanFence.h"

#include "VulkanInits.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanFence::VulkanFence(TRAP::Ref<VulkanDevice> device)
	: m_fence(VK_NULL_HANDLE), m_submitted(false), m_device(std::move(device))
{
	VkFenceCreateInfo info = VulkanInits::FenceCreateInfo();
	VkCall(vkCreateFence(m_device->GetVkDevice(), &info, nullptr, &m_fence));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFence::~VulkanFence()
{
	if(m_fence)
	{
		vkDestroyFence(m_device->GetVkDevice(), m_fence, nullptr);
		m_fence = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkFence& TRAP::Graphics::API::VulkanFence::GetVkFence()
{
	return m_fence;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanFence::IsSubmitted() const
{
	return m_submitted;
}
