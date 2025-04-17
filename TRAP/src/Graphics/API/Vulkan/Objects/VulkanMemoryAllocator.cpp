#include "TRAPPCH.h"
#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "VulkanInits.h"

namespace
{
	[[nodiscard]] VmaVulkanFunctions SetupVmaVulkanFunctions(const VmaAllocatorCreateInfo& allocatorCreateInfo)
	{
		VMA_ASSERT(allocatorCreateInfo.instance != VK_NULL_HANDLE);
		VMA_ASSERT(allocatorCreateInfo.device != VK_NULL_HANDLE);

		VmaVulkanFunctions outVulkanFunctions{};

		VkDeviceTable src{};
		VkLoadDeviceTable(src, allocatorCreateInfo.device);

#define COPY_GLOBAL_TO_VMA_FUNC(volkName, vmaName) if(!outVulkanFunctions.vmaName) outVulkanFunctions.vmaName = volkName;
#define COPY_DEVICE_TO_VMA_FUNC(volkName, vmaName) if(!outVulkanFunctions.vmaName) outVulkanFunctions.vmaName = src.volkName;

		COPY_GLOBAL_TO_VMA_FUNC(vkGetInstanceProcAddr, vkGetInstanceProcAddr)
		COPY_GLOBAL_TO_VMA_FUNC(vkGetDeviceProcAddr, vkGetDeviceProcAddr)
		COPY_GLOBAL_TO_VMA_FUNC(vkGetPhysicalDeviceProperties, vkGetPhysicalDeviceProperties)
		COPY_GLOBAL_TO_VMA_FUNC(vkGetPhysicalDeviceMemoryProperties, vkGetPhysicalDeviceMemoryProperties)
		COPY_DEVICE_TO_VMA_FUNC(vkAllocateMemory, vkAllocateMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkFreeMemory, vkFreeMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkMapMemory, vkMapMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkUnmapMemory, vkUnmapMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkFlushMappedMemoryRanges, vkFlushMappedMemoryRanges)
		COPY_DEVICE_TO_VMA_FUNC(vkInvalidateMappedMemoryRanges, vkInvalidateMappedMemoryRanges)
		COPY_DEVICE_TO_VMA_FUNC(vkBindBufferMemory, vkBindBufferMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkBindImageMemory, vkBindImageMemory)
		COPY_DEVICE_TO_VMA_FUNC(vkGetBufferMemoryRequirements, vkGetBufferMemoryRequirements)
		COPY_DEVICE_TO_VMA_FUNC(vkGetImageMemoryRequirements, vkGetImageMemoryRequirements)
		COPY_DEVICE_TO_VMA_FUNC(vkCreateBuffer, vkCreateBuffer)
		COPY_DEVICE_TO_VMA_FUNC(vkDestroyBuffer, vkDestroyBuffer)
		COPY_DEVICE_TO_VMA_FUNC(vkCreateImage, vkCreateImage)
		COPY_DEVICE_TO_VMA_FUNC(vkDestroyImage, vkDestroyImage)
		COPY_DEVICE_TO_VMA_FUNC(vkCmdCopyBuffer, vkCmdCopyBuffer)
#if VMA_VULKAN_VERSION >= 1001000
		if (allocatorCreateInfo.vulkanApiVersion >= VK_MAKE_VERSION(1, 1, 0))
		{
			COPY_GLOBAL_TO_VMA_FUNC(vkGetPhysicalDeviceMemoryProperties2, vkGetPhysicalDeviceMemoryProperties2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkGetBufferMemoryRequirements2, vkGetBufferMemoryRequirements2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkGetImageMemoryRequirements2, vkGetImageMemoryRequirements2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkBindBufferMemory2, vkBindBufferMemory2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkBindImageMemory2, vkBindImageMemory2KHR)
		}
#endif
#if VMA_VULKAN_VERSION >= 1003000
		if (allocatorCreateInfo.vulkanApiVersion >= VK_MAKE_VERSION(1, 3, 0))
		{
			COPY_DEVICE_TO_VMA_FUNC(vkGetDeviceBufferMemoryRequirements, vkGetDeviceBufferMemoryRequirements)
			COPY_DEVICE_TO_VMA_FUNC(vkGetDeviceImageMemoryRequirements, vkGetDeviceImageMemoryRequirements)
		}
#endif
#if VMA_KHR_MAINTENANCE4
		if((allocatorCreateInfo.flags & VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT) != 0)
		{
			COPY_DEVICE_TO_VMA_FUNC(vkGetDeviceBufferMemoryRequirementsKHR, vkGetDeviceBufferMemoryRequirements)
			COPY_DEVICE_TO_VMA_FUNC(vkGetDeviceImageMemoryRequirementsKHR, vkGetDeviceImageMemoryRequirements)
		}
#endif
#if VMA_DEDICATED_ALLOCATION
		if ((allocatorCreateInfo.flags & VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT) != 0)
		{
			COPY_DEVICE_TO_VMA_FUNC(vkGetBufferMemoryRequirements2KHR, vkGetBufferMemoryRequirements2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkGetImageMemoryRequirements2KHR, vkGetImageMemoryRequirements2KHR)
		}
#endif
#if VMA_BIND_MEMORY2
		if ((allocatorCreateInfo.flags & VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT) != 0)
		{
			COPY_DEVICE_TO_VMA_FUNC(vkBindBufferMemory2KHR, vkBindBufferMemory2KHR)
			COPY_DEVICE_TO_VMA_FUNC(vkBindImageMemory2KHR, vkBindImageMemory2KHR)
		}
#endif
#if VMA_MEMORY_BUDGET
		if ((allocatorCreateInfo.flags & VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT) != 0)
		{
			COPY_GLOBAL_TO_VMA_FUNC(vkGetPhysicalDeviceMemoryProperties2KHR, vkGetPhysicalDeviceMemoryProperties2KHR)
		}
#endif
#if VMA_EXTERNAL_MEMORY_WIN32
		if ((allocatorCreateInfo.flags & VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT) != 0)
		{
			COPY_DEVICE_TO_VMA_FUNC(vkGetMemoryWin32HandleKHR, vkGetMemoryWin32HandleKHR)
		}
#endif

#undef COPY_DEVICE_TO_VMA_FUNC
#undef COPY_GLOBAL_TO_VMA_FUNC

		return outVulkanFunctions;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanMemoryAllocator::VulkanMemoryAllocator(TRAP::Ref<VulkanDevice> device,
                                                                  TRAP::Ref<VulkanInstance> instance)
	: m_device(std::move(device)), m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanMemoryAllocator(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_instance, "VulkanMemoryAllocator(): Vulkan Instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanVMAPrefix, "Creating Allocator");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VmaAllocatorCreateInfo info = VulkanInits::VMAAllocatorCreateInfo(*m_device, m_instance->GetVkInstance());
	VmaVulkanFunctions vulkanFuncs = SetupVmaVulkanFunctions(info);
	info.pVulkanFunctions = &vulkanFuncs;

	VkCall(vmaCreateAllocator(&info, &m_allocator));
	TRAP_ASSERT(m_allocator, "VulkanMemoryAllocator(): Vulkan Allocator is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanVMAPrefix, "Destroying Allocator");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vmaDestroyAllocator(m_allocator);
}
