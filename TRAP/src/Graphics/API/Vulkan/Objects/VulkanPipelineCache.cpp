#include "TRAPPCH.h"
#include "VulkanPipelineCache.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "FS/FS.h"

TRAP::Graphics::API::VulkanPipelineCache::VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc)
	: m_cache(VK_NULL_HANDLE), m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice())
{
	TRAP_ASSERT(m_device);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Creating PipelineCache");
#endif

	VkPipelineCacheCreateInfo psoCacheCreateInfo;
	psoCacheCreateInfo = VulkanInits::PipelineCacheCreateInfo(desc.Data,
	                                                          PipelineCacheFlagsToVkPipelineCacheCreateFlags(desc.Flags));
	VkCall(vkCreatePipelineCache(m_device->GetVkDevice(), &psoCacheCreateInfo, nullptr, &m_cache));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipelineCache::~VulkanPipelineCache()
{
	TRAP_ASSERT(m_cache);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Destroying PipelineCache");
#endif

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

void TRAP::Graphics::API::VulkanPipelineCache::Save(const std::filesystem::path& path)
{
	std::vector<uint8_t> data{};
	std::size_t dataSize = 0;

	GetPipelineCacheData(&dataSize, nullptr);
	if (dataSize == 0)
		return;
	data.resize(dataSize);
	GetPipelineCacheData(&dataSize, data.data());

	if (!TRAP::FS::WriteFile(path, data))
		TP_ERROR(Log::RendererVulkanPipelineCachePrefix, "Saving of PipelineCache to path: \"",
		         path.generic_u8string(), "\" failed!");
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineCache TRAP::Graphics::API::VulkanPipelineCache::GetVkPipelineCache() const
{
	return m_cache;
}