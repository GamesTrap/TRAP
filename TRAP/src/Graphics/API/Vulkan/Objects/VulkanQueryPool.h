#ifndef TRAP_VULKANQUERYPOOL_H
#define TRAP_VULKANQUERYPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/QueryPool.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanQueryPool final : public QueryPool
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Query pool description.</param>
		explicit VulkanQueryPool(const RendererAPI::QueryPoolDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanQueryPool() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanQueryPool(const VulkanQueryPool&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanQueryPool& operator=(const VulkanQueryPool&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanQueryPool(VulkanQueryPool&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanQueryPool& operator=(VulkanQueryPool&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan query pool handle.
		/// </summary>
		/// <returns>Vulkan query pool handle</returns>
		VkQueryPool GetVkQueryPool() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan query type.
		/// </summary>
		/// <returns>Vulkan query type</returns>
		VkQueryType GetVkQueryType() const noexcept;
		/// <summary>
		/// Retrieve the number of queries managed by the pool.
		/// </summary>
		/// <returns>Number of queries</returns>
		uint32_t GetCount() const noexcept;

	private:
		VkQueryPool m_vkQueryPool;
		VkQueryType m_type;
		uint32_t m_count;
	};
}

#endif /*TRAP_VULKANQUERYPOOL_H*/