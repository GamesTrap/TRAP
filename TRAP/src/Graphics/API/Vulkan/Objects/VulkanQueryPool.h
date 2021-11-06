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
		/// Default Copy Constructor.
		/// </summary>
		VulkanQueryPool(const VulkanQueryPool&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanQueryPool& operator=(const VulkanQueryPool&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanQueryPool(VulkanQueryPool&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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