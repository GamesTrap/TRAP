#include "TRAPPCH.h"
#include "VulkanFence.h"

#include "VulkanInits.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/RendererAPI.h"

TRAP::Graphics::API::VulkanFence::VulkanFence(TRAP::Ref<VulkanDevice> device)
	: m_fence(VK_NULL_HANDLE), m_submitted(false), m_device(std::move(device))
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFencePrefix, "Creating Fence");
#endif
	
	VkFenceCreateInfo info = VulkanInits::FenceCreateInfo();
	VkCall(vkCreateFence(m_device->GetVkDevice(), &info, nullptr, &m_fence));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFence::~VulkanFence()
{
	if(m_fence)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanFencePrefix, "Destroying Fence");
#endif
		
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::FenceStatus TRAP::Graphics::API::VulkanFence::GetStatus()
{
	if(m_submitted)
	{
		const VkResult res = vkGetFenceStatus(m_device->GetVkDevice(), m_fence);
		if(res == VK_SUCCESS)
		{
			VkCall(vkResetFences(m_device->GetVkDevice(), 1, &m_fence));
			m_submitted = false;
		}

		return res == VK_SUCCESS ? RendererAPI::FenceStatus::Complete : RendererAPI::FenceStatus::Incomplete;
	}

	return RendererAPI::FenceStatus::NotSubmitted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFence::Wait()
{
	VkCall(vkWaitForFences(m_device->GetVkDevice(), 1, &m_fence, VK_TRUE, std::numeric_limits<uint32_t>::max()));
	VkCall(vkResetFences(m_device->GetVkDevice(), 1, &m_fence));

	m_submitted = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFence::WaitForFences(const TRAP::Scope<VulkanDevice>& device,
                                                     std::vector<VulkanFence>& fences)
{
	TRAP_ASSERT(device, "Device is nullptr");

	if (!fences.empty())
	{
		for(VulkanFence& f : fences)
			f.Wait();
	}
}