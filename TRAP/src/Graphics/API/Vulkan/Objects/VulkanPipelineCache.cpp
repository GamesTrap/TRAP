#include "TRAPPCH.h"
#include "VulkanPipelineCache.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "FileSystem/FileSystem.h"

TRAP::Graphics::API::VulkanPipelineCache::VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Destroying PipelineCache");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyPipelineCache(m_device->GetVkDevice(), m_cache, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipelineCache::GetPipelineCacheData(std::size_t* const size, void* const data) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(m_cache != nullptr)
		VkCall(vkGetPipelineCacheData(m_device->GetVkDevice(), m_cache, size, data));
}
