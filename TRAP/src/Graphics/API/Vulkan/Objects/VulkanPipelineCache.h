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
		/// @brief Constructor.
		/// @param desc Pipeline cache description.
		explicit VulkanPipelineCache(const RendererAPI::PipelineCacheDesc& desc);
		/// @brief Destructor.
		~VulkanPipelineCache() override;

		/// @brief Copy constructor.
		VulkanPipelineCache(const VulkanPipelineCache&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanPipelineCache& operator=(const VulkanPipelineCache&) noexcept = default;
		/// @brief Move constructor.
		VulkanPipelineCache(VulkanPipelineCache&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanPipelineCache& operator=(VulkanPipelineCache&&) noexcept = default;

		/// @brief Retrieve the cached pipeline data.
		///
		/// To retrieve the size of cached data call this function with data = nullptr.
		/// @param size Output: Size of the data.
		/// @param data Output: Pointer to store the data.
		void GetPipelineCacheData(usize* size, void* data) const override;

		/// @brief Retrieve the Vulkan pipeline cache handle.
		/// @return Vulkan pipeline cache handle
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