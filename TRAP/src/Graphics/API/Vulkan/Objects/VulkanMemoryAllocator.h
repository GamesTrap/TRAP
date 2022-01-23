#ifndef TRAP_VULKANMEMORYALLOCATOR_H
#define TRAP_VULKANMEMORYALLOCATOR_H

#include <vk_mem_alloc.h>

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanDevice;

	class VulkanMemoryAllocator
	{
	public:
		VulkanMemoryAllocator(const TRAP::Ref<VulkanDevice>& device, const TRAP::Ref<VulkanInstance>& instance);
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

		VmaAllocator GetVMAAllocator() const;

	private:
		VmaAllocator m_allocator;
	};
}

#endif /*TRAP_VULKANMEMORYALLOCATOR_H*/