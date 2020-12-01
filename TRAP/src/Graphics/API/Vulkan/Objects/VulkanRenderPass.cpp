#include "TRAPPCH.h"
#include "VulkanRenderPass.h"

#include "VulkanDevice.h"

TRAP::Graphics::API::VulkanRenderPass::VulkanRenderPass(TRAP::Ref<VulkanDevice> device)
	: m_renderPass(VK_NULL_HANDLE), m_device(std::move(device))
{
	TRAP_ASSERT(m_device, "device is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Creating RenderPass");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderPass::~VulkanRenderPass()
{
	if (m_renderPass)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Destroying RenderPass");
#endif
		vkDestroyRenderPass(m_device->GetVkDevice(), m_renderPass, nullptr);
		m_renderPass = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass& TRAP::Graphics::API::VulkanRenderPass::GetVkRenderPass()
{
	return m_renderPass;
}