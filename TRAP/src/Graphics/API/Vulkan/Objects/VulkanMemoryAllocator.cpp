#include "TRAPPCH.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "VulkanMemoryAllocator.h"

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
	vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
	vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2;

	//Memory Budget
	if(VulkanRenderer::s_memoryBudgetExtension)
		vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;

	if(VulkanRenderer::s_maintenance4Extension)
	{
		vulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirementsKHR;
		vulkanFunctions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirementsKHR;
	}

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
