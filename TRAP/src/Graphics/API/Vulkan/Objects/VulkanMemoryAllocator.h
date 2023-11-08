#ifndef TRAP_VULKANMEMORYALLOCATOR_H
#define TRAP_VULKANMEMORYALLOCATOR_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <vk_mem_alloc.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Base.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanDevice;

	class VulkanMemoryAllocator
	{
	public:
		/// @brief Constructor.
		/// @param device Logical Vulkan device.
		/// @param instance Vulkan instance.
		VulkanMemoryAllocator(const TRAP::Ref<VulkanDevice>& device, const TRAP::Ref<VulkanInstance>& instance);
		/// @brief Destructor.
		~VulkanMemoryAllocator();

		/// @brief Copy constructor.
		consteval VulkanMemoryAllocator(const VulkanMemoryAllocator&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) noexcept = delete;
		/// @brief Move constructor.
		constexpr VulkanMemoryAllocator(VulkanMemoryAllocator&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) noexcept = default;

		/// @brief Retrieve the Vulkan memory allocator (VMA) allocator handle.
		/// @return VMA allocator handle.
		[[nodiscard]] constexpr VmaAllocator GetVMAAllocator() const noexcept;

	private:
		VmaAllocator m_allocator = VK_NULL_HANDLE;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VmaAllocator TRAP::Graphics::API::VulkanMemoryAllocator::GetVMAAllocator() const noexcept
{
	return m_allocator;
}

#endif /*TRAP_VULKANMEMORYALLOCATOR_H*/