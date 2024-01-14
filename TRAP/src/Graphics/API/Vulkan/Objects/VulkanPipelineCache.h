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
		consteval VulkanPipelineCache(const VulkanPipelineCache&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanPipelineCache& operator=(const VulkanPipelineCache&) noexcept = delete;
		/// @brief Move constructor.
		VulkanPipelineCache(VulkanPipelineCache&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanPipelineCache& operator=(VulkanPipelineCache&&) noexcept = default;

		/// @brief Retrieve the cached pipeline data in bytes.
		/// @return Pipeline cache data as bytes.
		[[nodiscard]] std::vector<u8> GetPipelineCacheData() const override;

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