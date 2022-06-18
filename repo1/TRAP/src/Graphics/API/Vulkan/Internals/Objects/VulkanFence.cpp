#include "TRAPPCH.h"
#include "VulkanFence.h"

#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"

TRAP::Graphics::API::Vulkan::Fence::Fence(Device* device, const bool signaled)
	: m_fence(), m_device(device)
{
	VkFenceCreateInfo fenceCreateInfo = Initializers::FenceCreateInfo(signaled);
	VkCall(vkCreateFence(m_device->GetDevice(), &fenceCreateInfo, nullptr, &m_fence));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Fence::~Fence()
{
	if(m_fence)
	{
		vkDestroyFence(m_device->GetDevice(), m_fence, nullptr);
		m_fence = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkFence& TRAP::Graphics::API::Vulkan::Fence::GetFence()
{
	return m_fence;
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::Graphics::API::Vulkan::Fence::GetStatus() const
{
	const VkResult result = vkGetFenceStatus(m_device->GetDevice(), m_fence);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Fence::Reset() const
{
	VkCall(vkResetFences(m_device->GetDevice(), 1, &m_fence));
}