#ifndef TRAP_VULKANPIPELINECACHE_H
#define TRAP_VULKANPIPELINECACHE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/PipelineCache.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipelineCache final : public PipelineCache
	{
	public:
		explicit VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc);
		~VulkanPipelineCache() override;

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VulkanPipelineCache(const VulkanPipelineCache&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanPipelineCache& operator=(const VulkanPipelineCache&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanPipelineCache(VulkanPipelineCache&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanPipelineCache& operator=(VulkanPipelineCache&&) = default;

		void GetPipelineCacheData(std::size_t* size, void* data) const override;

		void Save(const std::string& virtualOrPhysicalPath) override;

		VkPipelineCache GetVkPipelineCache() const;

	private:
		VkPipelineCache m_cache;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANPIPELINECACHE_H*/