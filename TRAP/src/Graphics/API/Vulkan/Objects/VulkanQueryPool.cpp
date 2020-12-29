#include "TRAPPCH.h"
#include "VulkanQueryPool.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanQueryPool::VulkanQueryPool(TRAP::Ref<VulkanDevice> device, const RendererAPI::QueryPoolDesc& desc)
	: m_vkQueryPool(VK_NULL_HANDLE), m_type(), m_count(), m_device(std::move(device))
{
	TRAP_ASSERT(m_device);
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Creating QueryPool");
#endif

	m_type = QueryTypeToVkQueryType(desc.Type);
	m_count = desc.QueryCount;

	VkQueryPoolCreateInfo info = VulkanInits::QueryPoolCreateInfo(desc.QueryCount, QueryTypeToVkQueryType(desc.Type));
	VkCall(vkCreateQueryPool(m_device->GetVkDevice(), &info, nullptr, &m_vkQueryPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanQueryPool::~VulkanQueryPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Destroying QueryPool");
#endif

	TRAP_ASSERT(m_vkQueryPool);
	vkDestroyQueryPool(m_device->GetVkDevice(), m_vkQueryPool, nullptr);
}

VkQueryPool& TRAP::Graphics::API::VulkanQueryPool::GetVkQueryPool()
{
	return m_vkQueryPool;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueryType TRAP::Graphics::API::VulkanQueryPool::GetVkQueryType() const
{
	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanQueryPool::GetCount() const
{
	return m_count;
}