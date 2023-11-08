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
		/// @brief Constructor.
		/// @param desc Query pool description.
		explicit VulkanQueryPool(const RendererAPI::QueryPoolDesc& desc);
		/// @brief Destructor.
		~VulkanQueryPool() override;

		/// @brief Copy constructor.
		consteval VulkanQueryPool(const VulkanQueryPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanQueryPool& operator=(const VulkanQueryPool&) noexcept = delete;
		/// @brief Move constructor.
		constexpr VulkanQueryPool(VulkanQueryPool&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanQueryPool& operator=(VulkanQueryPool&&) noexcept = default;

		/// @brief Retrieve the Vulkan query pool handle.
		/// @return Vulkan query pool handle
		[[nodiscard]] constexpr VkQueryPool GetVkQueryPool() const noexcept;
		/// @brief Retrieve the Vulkan query type.
		/// @return Vulkan query type
		[[nodiscard]] constexpr VkQueryType GetVkQueryType() const noexcept;
		/// @brief Retrieve the number of queries managed by the pool.
		/// @return Number of queries
		[[nodiscard]] constexpr u32 GetCount() const noexcept;

	private:
		VkQueryPool m_vkQueryPool = VK_NULL_HANDLE;
		VkQueryType m_type;
		u32 m_count;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueryPool TRAP::Graphics::API::VulkanQueryPool::GetVkQueryPool() const noexcept
{
	return m_vkQueryPool;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueryType TRAP::Graphics::API::VulkanQueryPool::GetVkQueryType() const noexcept
{
	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanQueryPool::GetCount() const noexcept
{
	return m_count;
}

#endif /*TRAP_VULKANQUERYPOOL_H*/