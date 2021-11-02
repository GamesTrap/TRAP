#ifndef TRAP_VULKANMEMORYALLOCATOR_H
#define TRAP_VULKANMEMORYALLOCATOR_H

#include "Graphics/API/Vulkan/Utils/VMA.h"

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
		/// Default Copy Constructor.
		/// </summary>
		VulkanMemoryAllocator(const VulkanMemoryAllocator&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanMemoryAllocator(VulkanMemoryAllocator&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) = default;

		VmaAllocator& GetVMAAllocator();

	private:
		VmaAllocator m_allocator;
	};
}

#endif /*TRAP_VULKANMEMORYALLOCATOR_H*/