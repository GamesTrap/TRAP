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
		explicit VulkanQueryPool(const RendererAPI::QueryPoolDesc& desc);
		~VulkanQueryPool() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanQueryPool(const VulkanQueryPool&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanQueryPool& operator=(const VulkanQueryPool&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanQueryPool(VulkanQueryPool&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanQueryPool& operator=(VulkanQueryPool&&) = default;

		VkQueryPool GetVkQueryPool() const;
		VkQueryType GetVkQueryType() const;
		uint32_t GetCount() const;

	private:
		VkQueryPool m_vkQueryPool;
		VkQueryType m_type;
		uint32_t m_count;
	};
}

#endif /*TRAP_VULKANQUERYPOOL_H*/