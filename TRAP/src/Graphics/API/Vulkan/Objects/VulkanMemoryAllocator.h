#ifndef _TRAP_VULKANMEMORYALLOCATOR_H_
#define _TRAP_VULKANMEMORYALLOCATOR_H_

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

		VmaAllocator& GetVMAAllocator();

	private:
		VmaAllocator m_allocator;
	};
}

#endif /*_TRAP_VULKANMEMORYALLOCATOR_H_*/