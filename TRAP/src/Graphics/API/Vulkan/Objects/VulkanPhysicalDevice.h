#ifndef _TRAP_VULKANPHYSICALDEVICE_H_
#define _TRAP_VULKANPHYSICALDEVICE_H_
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(const TRAP::Ref<VulkanInstance>& instance, 
			                 const std::array<uint8_t, 16>& physicalDeviceUUID);
		~VulkanPhysicalDevice();
		VulkanPhysicalDevice(const VulkanPhysicalDevice&) = default;
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = default;
		VulkanPhysicalDevice(VulkanPhysicalDevice&&) = default;
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) = default;
		
		VkPhysicalDevice& GetVkPhysicalDevice();
		const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const;
		const VkPhysicalDeviceSubgroupProperties& GetVkPhysicalDeviceSubgroupProperties() const;
		const VkPhysicalDeviceIDPropertiesKHR& GetVkPhysicalDeviceIDProperties() const;
		const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const;
		const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const;
		const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const;
		bool IsExtensionSupported(const std::string& extension);
		
		const std::vector<VkExtensionProperties>& GetAvailablePhysicalDeviceExtensions();
		
		const std::array<uint8_t, 16>& GetPhysicalDeviceUUID() const;

		void RetrievePhysicalDeviceFragmentShaderInterlockFeatures();
		
		static std::multimap<uint32_t, std::array<uint8_t, 16>> GetAllRatedPhysicalDevices(
			const TRAP::Ref<VulkanInstance>& instance);

		static VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance>& instance,
			const std::array<uint8_t, 16>& physicalDeviceUUID);
		
	private:
		friend bool TRAP::Graphics::RendererAPI::IsVulkanCapable();
		static std::multimap<uint32_t, std::array<uint8_t, 16>> GetAllRatedPhysicalDevices(
			const VkInstance& instance);
		
		static std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(const VkInstance& instance);

		static VkPhysicalDeviceIDPropertiesKHR RetrievePhysicalDeviceIDProperties(const VkPhysicalDevice& physicalDevice);
		static VkPhysicalDeviceSubgroupProperties RetrievePhysicalDeviceSubgroupProperties(const VkPhysicalDevice& physicalDevice);

		static void RatePhysicalDevices(const std::vector<VkPhysicalDevice>& physicalDevices);
		
		void LoadAllPhysicalDeviceExtensions();
		
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceSubgroupProperties m_physicalDeviceSubgroupProperties;
		VkPhysicalDeviceIDPropertiesKHR m_physicalDeviceIDProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT m_physicalDeviceFragmentShaderInterlockFeatures;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

		std::array<uint8_t, 16> m_deviceUUID;

		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions;

		static std::multimap<uint32_t, std::array<uint8_t, 16>> s_availablePhysicalDeviceUUIDs;
	};
}

#endif /*_TRAP_VULKANPHYSICALDEVICE_H_*/