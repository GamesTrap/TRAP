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
		/// Copy constructor.
		/// </summary>
		VulkanPipelineCache(const VulkanPipelineCache&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanPipelineCache& operator=(const VulkanPipelineCache&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanPipelineCache(VulkanPipelineCache&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanPipelineCache& operator=(VulkanPipelineCache&&) = default;

		void GetPipelineCacheData(std::size_t* size, void* data) const override;

		void Save(const std::filesystem::path& path) override;

		VkPipelineCache GetVkPipelineCache() const;

	private:
		VkPipelineCache m_cache;

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*TRAP_VULKANPIPELINECACHE_H*/