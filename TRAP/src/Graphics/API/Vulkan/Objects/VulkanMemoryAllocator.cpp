#include "TRAPPCH.h"
#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <vk_mem_alloc.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "VulkanInits.h"

namespace
{
	[[nodiscard]] VmaVulkanFunctions SetupVmaVulkanFunctions(const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		VmaVulkanFunctions vulkanFunctions;

		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
		vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
		vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
		vulkanFunctions.vkFreeMemory = vkFreeMemory;
		vulkanFunctions.vkMapMemory = vkMapMemory;
		vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
		vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
		vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
		vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
		vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
		vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
		vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
		vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
		vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
		vulkanFunctions.vkCreateImage = vkCreateImage;
		vulkanFunctions.vkDestroyImage = vkDestroyImage;
		vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;

		//Dedicated Allocation
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
		vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
		vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;
#endif /*VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000*/

		//Bind Memory 2
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
		vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
		vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2;
#endif /*VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000*/

		//Memory Budget
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
		if(physicalDevice.IsExtensionSupported(TRAP::Graphics::API::VulkanPhysicalDeviceExtension::MemoryBudget))
			vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
#endif /*VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000*/

#if VMA_KHR_MAINTENANCE4 || VMA_VULKAN_VERSION >= 1003000
		if(physicalDevice.IsExtensionSupported(TRAP::Graphics::API::VulkanPhysicalDeviceExtension::Maintenance4))
		{
			vulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirementsKHR;
			vulkanFunctions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirementsKHR;
		}
#endif /*VMA_KHR_MAINTENANCE4 || VMA_VULKAN_VERSION >= 1003000*/

		return vulkanFunctions;
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

	VmaVulkanFunctions vulkanFunctions = SetupVmaVulkanFunctions(m_device->GetPhysicalDevice());

	const VmaAllocatorCreateInfo info = VulkanInits::VMAAllocatorCreateInfo(*m_device, m_instance->GetVkInstance(),
	                                                                        vulkanFunctions);

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
