#ifndef TRAP_VULKANMEMORYALLOCATOR_H
#define TRAP_VULKANMEMORYALLOCATOR_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <vk_mem_alloc.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

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
		VulkanMemoryAllocator(const VulkanMemoryAllocator&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanMemoryAllocator(VulkanMemoryAllocator&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) = default;

		/// <summary>
		/// Retrieve the Vulkan memory allocator (VMA) allocator handle.
		/// </summary>
		/// <returns>VMA allocator handle.</returns>
		VmaAllocator GetVMAAllocator() const;

	private:
		VmaAllocator m_allocator;
	};
}

#endif /*TRAP_VULKANMEMORYALLOCATOR_H*/