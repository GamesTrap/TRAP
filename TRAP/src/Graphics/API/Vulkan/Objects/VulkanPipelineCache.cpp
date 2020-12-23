#include "TRAPPCH.h"
#include "VulkanPipelineCache.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanPipelineCache::VulkanPipelineCache(TRAP::Ref<VulkanDevice> device, const RendererAPI::PipelineCacheDesc& desc)
	: m_cache(VK_NULL_HANDLE), m_device(std::move(device))
{
	TRAP_ASSERT(m_device);
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Creating PipelineCache");
#endif

	VkPipelineCacheCreateInfo psoCacheCreateInfo = VulkanInits::PipelineCacheCreateInfo(desc.Size, desc.Data, PipelineCacheFlagsToVkPipelineCacheCreateFlags(desc.Flags));
	VkCall(vkCreatePipelineCache(m_device->GetVkDevice(), &psoCacheCreateInfo, nullptr, &m_cache));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipelineCache::~VulkanPipelineCache()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Creating PipelineCache");
#endif

	if(m_cache)
		vkDestroyPipelineCache(m_device->GetVkDevice(), m_cache, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipelineCache::GetPipelineCacheData(std::size_t* size, void* data) const
{
	TRAP_ASSERT(m_device);

	if(m_cache)
		VkCall(vkGetPipelineCacheData(m_device->GetVkDevice(), m_cache, size, data));
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineCache TRAP::Graphics::API::VulkanPipelineCache::GetVkPipelineCache() const
{
	return m_cache;
}