#include "TRAPPCH.h"

#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <vk_mem_alloc.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "VulkanInits.h"

TRAP::Graphics::API::VulkanMemoryAllocator::VulkanMemoryAllocator(const TRAP::Ref<VulkanDevice>& device,
                                                                  const TRAP::Ref<VulkanInstance>& instance)
	: m_allocator(nullptr)
{
	TRAP_ASSERT(device, "device is nullptr");
	TRAP_ASSERT(instance, "instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanVMAPrefix, "Creating Allocator");
#endif

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
	vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
	vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;

	//Bind Memory 2
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
	vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
	vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2;
#endif

	//Memory Budget
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
	if(VulkanRenderer::s_memoryBudgetExtension)
		vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
#endif

#if VMA_VULKAN_VERSION >= 1003000
	if(VulkanRenderer::s_maintenance4Extension)
	{
		vulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirementsKHR;
		vulkanFunctions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirementsKHR;
	}
#endif

	VmaAllocatorCreateInfo info = VulkanInits::VMAAllocatorCreateInfo(device->GetVkDevice(),
		                                                              device->GetPhysicalDevice()->GetVkPhysicalDevice(),
		                                                              instance->GetVkInstance(), vulkanFunctions);

	VkCall(vmaCreateAllocator(&info, &m_allocator));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
	TRAP_ASSERT(m_allocator);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanVMAPrefix, "Destroying Allocator");
#endif
	vmaDestroyAllocator(m_allocator);
	m_allocator = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VmaAllocator TRAP::Graphics::API::VulkanMemoryAllocator::GetVMAAllocator() const
{
	return m_allocator;
}
