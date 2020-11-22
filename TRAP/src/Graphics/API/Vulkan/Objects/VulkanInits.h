#ifndef _TRAP_VULKANINITS_H_
#define _TRAP_VULKANINITS_H_

namespace TRAP::Graphics::API::VulkanInits
{
	VkApplicationInfo ApplicationInfo(const std::string& appName);
	
	VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo, 
									const std::vector<const char*>& instanceLayers,
	                                const std::vector<const char*>& instanceExtensions);

	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);

	VkDeviceCreateInfo DeviceCreateInfo(const void* pNext,
	                                    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	                                    const std::vector<const char*>& deviceExtensions);
}

#endif /*_TRAP_VULKANINITS_H_*/