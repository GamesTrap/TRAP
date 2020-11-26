#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/VulkanRenderer.h"

VkApplicationInfo TRAP::Graphics::API::VulkanInits::ApplicationInfo(const std::string& appName)
{
	VkApplicationInfo info;

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pNext = nullptr;
	info.pApplicationName = appName.data();
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "TRAP";
	info.engineVersion = TRAP_VERSION;
	info.apiVersion = VK_API_VERSION_1_2;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkInstanceCreateInfo TRAP::Graphics::API::VulkanInits::InstanceCreateInfo(const VkApplicationInfo& appInfo,
	const std::vector<const char*>& instanceLayers,
	const std::vector<const char*>& instanceExtensions)
{
	VkInstanceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pApplicationInfo = &appInfo;
	info.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
	info.ppEnabledLayerNames = instanceLayers.data();
	info.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	info.ppEnabledExtensionNames = instanceExtensions.data();
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDebugUtilsMessengerCreateInfoEXT TRAP::Graphics::API::VulkanInits::DebugUtilsMessengerCreateInfo(
	const PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
	VkDebugUtilsMessengerCreateInfoEXT info;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = 0;
	info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	info.pfnUserCallback = callback;
	info.pUserData = nullptr;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceCreateInfo TRAP::Graphics::API::VulkanInits::DeviceCreateInfo(const void* pNext,
                                                                      const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
																	  const std::vector<const char*>& deviceExtensions)
{
	VkDeviceCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = pNext;
	info.flags = 0;
	info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	info.pQueueCreateInfos = queueCreateInfos.data();
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = nullptr;
	info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	info.ppEnabledExtensionNames = deviceExtensions.data();
	info.pEnabledFeatures = nullptr;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VmaAllocatorCreateInfo TRAP::Graphics::API::VulkanInits::VMAAllocatorCreateInfo(VkDevice device,
	VkPhysicalDevice physicalDevice,
	VkInstance instance,
	const VmaVulkanFunctions& vulkanFunctions)
{
	VmaAllocatorCreateInfo info{};

	info.device = device;
	info.physicalDevice = physicalDevice;
	info.instance = instance;
	info.flags = 0;

	if (!VulkanRenderer::s_renderdocCapture)
		info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

	info.pVulkanFunctions = &vulkanFunctions;
	info.pAllocationCallbacks = nullptr;
	info.vulkanApiVersion = VK_API_VERSION_1_2;
	
	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorPoolCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(
	const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes, const uint32_t numDescriptorSets)
{
	VkDescriptorPoolCreateInfo info;

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	info.pPoolSizes = descriptorPoolSizes.data();
	info.maxSets = numDescriptorSets;
	
	return info;
}