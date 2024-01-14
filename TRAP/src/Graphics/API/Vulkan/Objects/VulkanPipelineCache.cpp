#include "TRAPPCH.h"
#include "VulkanPipelineCache.h"

#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"

TRAP::Graphics::API::VulkanPipelineCache::VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanPipelineCache(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Creating PipelineCache");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const VkPipelineCacheCreateInfo psoCacheCreateInfo = VulkanInits::PipelineCacheCreateInfo(desc.Data,
	                                                     PipelineCacheFlagsToVkPipelineCacheCreateFlags(desc.Flags));
	VkCall(vkCreatePipelineCache(m_device->GetVkDevice(), &psoCacheCreateInfo, nullptr, &m_cache));
	TRAP_ASSERT(m_cache, "VulkanPipelineCache(): PipelineCache is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipelineCache::~VulkanPipelineCache()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Destroying PipelineCache");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyPipelineCache(m_device->GetVkDevice(), m_cache, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<u8> TRAP::Graphics::API::VulkanPipelineCache::GetPipelineCacheData() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(m_cache == nullptr)
		return {};

	usize dataSize = 0;
	VkCall(vkGetPipelineCacheData(m_device->GetVkDevice(), m_cache, &dataSize, nullptr));

	if(dataSize == 0)
		return {};

	std::vector<u8> data(dataSize);
	VkCall(vkGetPipelineCacheData(m_device->GetVkDevice(), m_cache, &dataSize, data.data()));

	return data;
}
