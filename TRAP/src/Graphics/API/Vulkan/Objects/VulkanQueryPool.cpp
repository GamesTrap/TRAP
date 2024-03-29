#include "TRAPPCH.h"
#include "VulkanQueryPool.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanQueryPool::VulkanQueryPool(const RendererAPI::QueryPoolDesc& desc)
	: m_type(QueryTypeToVkQueryType(desc.Type)), m_count(desc.QueryCount)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const TRAP::Ref<VulkanDevice> device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	TRAP_ASSERT(device, "VulkanQueryPool(): Vulkan Device is nullptr!");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Creating QueryPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const VkQueryPoolCreateInfo info = VulkanInits::QueryPoolCreateInfo(m_count, m_type);
	VkCall(vkCreateQueryPool(device->GetVkDevice(), &info, nullptr, &m_vkQueryPool));
	TRAP_ASSERT(m_vkQueryPool, "VulkanQueryPool(): Vulkan QueryPool is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanQueryPool::~VulkanQueryPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanQueryPoolPrefix, "Destroying QueryPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyQueryPool(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()->GetVkDevice(),
	                   m_vkQueryPool, nullptr);
}
