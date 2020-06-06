#ifndef _TRAP_VULKANPHYSICALDEVICE_H_
#define _TRAP_VULKANPHYSICALDEVICE_H_

#include "Graphics/API/Vulkan/Internals/VulkanQueueFamilyIndices.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API::Vulkan
{
	class Surface;
	class Instance;

	class PhysicalDevice
	{
	public:
		PhysicalDevice(VkPhysicalDevice device, QueueFamilyIndices indices);
		~PhysicalDevice();
		PhysicalDevice(const PhysicalDevice&) = default;
		PhysicalDevice& operator=(const PhysicalDevice&) = default;
		PhysicalDevice(PhysicalDevice&&) = default;
		PhysicalDevice& operator=(PhysicalDevice&&) = default;
		
		QueueFamilyIndices& GetQueueFamilyIndices();
		VkPhysicalDevice& GetPhysicalDevice();
		VkPhysicalDeviceProperties& GetPhysicalDeviceProperties();
		VkPhysicalDeviceVulkan11Properties& GetPhysicalDeviceVulkan11Properties();
		VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures();
		VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties();
		VkFormatProperties GetPhysicalDeviceFormatProperties(VkFormat format) const;
		std::string GetPhysicalDeviceName() const;
		const std::vector<VkExtensionProperties>& GetAvailableExtensions() const;
		bool IsExtensionSupported(const char* extension);
		bool IsRayTracingSupported() const;

		//Returns a multimap containing the PhysicalDevices and a score with the PhysicalDevices capabilities. Higher score means more capabilities.
		static std::multimap<int32_t, PhysicalDevice> GetAllAvailableGraphicPhysicalDevices(const Scope<Instance>& instance, INTERNAL::WindowingAPI::InternalWindow* window);
		
	private:
		static std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices(const Scope<Instance>& instance);
		static bool PhysicalDeviceSupports(VkPhysicalDevice& device, QueueFamilyIndices& familyIndices);
		static bool SupportsQueueFamily(VkPhysicalDevice& device, QueueFamilyIndices& familyIndices);

		VkPhysicalDevice m_device;
		QueueFamilyIndices m_familyIndices;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceVulkan11Properties m_deviceProperties11;
		VkPhysicalDeviceFeatures m_deviceFeatures;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;
		std::vector<VkExtensionProperties> m_deviceExtensions;
		bool m_rayTracing;

		static Instance* s_instance;
		static std::multimap<int32_t, PhysicalDevice> s_availableGraphicPhysicalDevices;
	};
}

#endif /*_TRAP_VULKANPHYSICALDEVICE_H_*/