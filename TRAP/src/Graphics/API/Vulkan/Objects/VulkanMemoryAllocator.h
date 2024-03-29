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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Logical Vulkan device.</param>
		/// <param name="instance">Vulkan instance.</param>
		VulkanMemoryAllocator(const TRAP::Ref<VulkanDevice>& device, const TRAP::Ref<VulkanInstance>& instance);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanMemoryAllocator();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanMemoryAllocator(const VulkanMemoryAllocator&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr VulkanMemoryAllocator(VulkanMemoryAllocator&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan memory allocator (VMA) allocator handle.
		/// </summary>
		/// <returns>VMA allocator handle.</returns>
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