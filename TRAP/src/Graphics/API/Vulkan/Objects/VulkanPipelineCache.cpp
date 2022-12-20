#include "TRAPPCH.h"
#include "VulkanPipelineCache.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "FileSystem/FileSystem.h"

TRAP::Graphics::API::VulkanPipelineCache::VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc)
	: m_cache(VK_NULL_HANDLE), m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice())
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanPipelineCache(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Creating PipelineCache");
#endif

	const VkPipelineCacheCreateInfo psoCacheCreateInfo = VulkanInits::PipelineCacheCreateInfo(desc.Data,
	                                                     PipelineCacheFlagsToVkPipelineCacheCreateFlags(desc.Flags));
	VkCall(vkCreatePipelineCache(m_device->GetVkDevice(), &psoCacheCreateInfo, nullptr, &m_cache));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPipelineCache::~VulkanPipelineCache()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_cache, "~VulkanPipelineCache(): PipelineCache is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPipelineCachePrefix, "Destroying PipelineCache");
#endif

	vkDestroyPipelineCache(m_device->GetVkDevice(), m_cache, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipelineCache::GetPipelineCacheData(std::size_t* const size, void* const data) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanPipelineCache::GetPipelineCacheData(): Vulkan Device is nullptr");

	if(m_cache)
		VkCall(vkGetPipelineCacheData(m_device->GetVkDevice(), m_cache, size, data));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPipelineCache::Save(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::vector<uint8_t> data{};
	std::size_t dataSize = 0;

	GetPipelineCacheData(&dataSize, nullptr);
	if (dataSize == 0)
		return;
	data.resize(dataSize);
	GetPipelineCacheData(&dataSize, data.data());

	if (!TRAP::FileSystem::WriteFile(path, data))
		TP_ERROR(Log::RendererVulkanPipelineCachePrefix, "Saving of PipelineCache to path: \"",
		         path.u8string(), "\" failed!");
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineCache TRAP::Graphics::API::VulkanPipelineCache::GetVkPipelineCache() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_cache;
}