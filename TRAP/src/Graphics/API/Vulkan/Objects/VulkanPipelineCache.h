#ifndef TRAP_VULKANPIPELINECACHE_H
#define TRAP_VULKANPIPELINECACHE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipelineCache final : public PipelineCache
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Pipeline cache description.</param>
		explicit VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanPipelineCache() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanPipelineCache(const VulkanPipelineCache&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanPipelineCache& operator=(const VulkanPipelineCache&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanPipelineCache(VulkanPipelineCache&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanPipelineCache& operator=(VulkanPipelineCache&&) noexcept = default;

		/// <summary>
		/// Retrieve the cached pipeline data.
		///
		/// To retrieve the size of cached data call this function with data = nullptr.
		/// </summary>
		/// <param name="size">Output: Size of the data.</param>
		/// <param name="data">Output: Pointer to store the data.</param>
		void GetPipelineCacheData(usize* size, void* data) const override;

		/// <summary>
		/// Retrieve the Vulkan pipeline cache handle.
		/// </summary>
		/// <returns>Vulkan pipeline cache handle</returns>
		[[nodiscard]] constexpr VkPipelineCache GetVkPipelineCache() const noexcept;

	private:
		VkPipelineCache m_cache = VK_NULL_HANDLE;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineCache TRAP::Graphics::API::VulkanPipelineCache::GetVkPipelineCache() const noexcept
{
	return m_cache;
}

#endif /*TRAP_VULKANPIPELINECACHE_H*/