#ifndef _TRAP_VULKANPIPELINECACHE_H_
#define _TRAP_VULKANPIPELINECACHE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanPipelineCache
	{
	public:
		VulkanPipelineCache(TRAP::Ref<VulkanDevice> device, const RendererAPI::PipelineCacheDesc& desc);
		~VulkanPipelineCache();

		void GetPipelineCacheData(std::size_t* size, void* data) const;

		VkPipelineCache GetVkPipelineCache() const;
		
	private:
		VkPipelineCache m_cache;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANPIPELINECACHE_H_*/