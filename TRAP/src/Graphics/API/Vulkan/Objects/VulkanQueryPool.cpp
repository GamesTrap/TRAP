#include "TRAPPCH.h"
#include "VulkanQueryPool.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanQueryPool::VulkanQueryPool(const RendererAPI::QueryPoolDesc& desc)
	: m_vkQueryPool(VK_NULL_HANDLE), m_type(), m_count()
{
	ZoneScopedC(tracy::Color::Red);

	const TRAP::Ref<VulkanDevice> device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	TRAP_ASSERT(device);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Creating QueryPool");
#endif

	m_type = QueryTypeToVkQueryType(desc.Type);
	m_count = desc.QueryCount;

	const VkQueryPoolCreateInfo info = VulkanInits::QueryPoolCreateInfo(desc.QueryCount,
	                                                                    QueryTypeToVkQueryType(desc.Type));
	VkCall(vkCreateQueryPool(device->GetVkDevice(), &info, nullptr, &m_vkQueryPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanQueryPool::~VulkanQueryPool()
{
	ZoneScopedC(tracy::Color::Red);

	TRAP_ASSERT(m_vkQueryPool);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Destroying QueryPool");
#endif

	vkDestroyQueryPool(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()->GetVkDevice(),
	                   m_vkQueryPool, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueryPool TRAP::Graphics::API::VulkanQueryPool::GetVkQueryPool() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_vkQueryPool;
}

//-------------------------------------------------------------------------------------------------------------------//

VkQueryType TRAP::Graphics::API::VulkanQueryPool::GetVkQueryType() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanQueryPool::GetCount() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_count;
}